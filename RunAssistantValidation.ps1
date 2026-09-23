param(
    [ValidateSet('Debug','Release')][string]$Configuration = 'Debug',
    [ValidateSet('Embedded','McpAttach')][string]$Scope = 'Embedded',
    [switch]$Launch,
    [switch]$Live,
    [switch]$Broader
)
$ErrorActionPreference = 'Stop'
$root = $PSScriptRoot
$output = Join-Path $root 'build'
$evidence = Join-Path $output ('Assistant-' + $Configuration + '-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
New-Item -ItemType Directory -Path $evidence -Force | Out-Null
$umk = 'E:\upp-18468\umk.exe'
$records = @()
function Record([string]$name,[string]$status) {
    $script:records += [pscustomobject]@{Name=$name;Status=$status}
    $script:records | ConvertTo-Json | Set-Content (Join-Path $evidence 'results.json')
    Write-Host "$name : $status"
}
function Build([string]$package,[string]$name,[bool]$gui) {
    $exe = Join-Path $output ($name + '.exe')
    $log = Join-Path $evidence ($name + '.build.log')
    $flags = if($Configuration -eq 'Release') {'-br'} else {'-b'}
    $arguments = @('github',$package,'CLANGx64',$flags)
    if($gui) {$arguments += '+GUI'}
    $arguments += $exe
    & $umk @arguments *> $log
    if($LASTEXITCODE -ne 0 -or !(Test-Path $exe)) { Record $name 'FAIL build'; Get-Content $log -Tail 70; throw "Build failed: $package" }
    Record ($name + ' build') 'PASS'
    return $exe
}
function Test([string]$exe) {
    $name = [IO.Path]::GetFileNameWithoutExtension($exe)
    $stdout = Join-Path $evidence ($name+'.stdout.log')
    $stderr = Join-Path $evidence ($name+'.stderr.log')
    $p = Start-Process -FilePath $exe -WorkingDirectory $evidence -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    $null = $p.Handle
    if(!$p.WaitForExit(240000)) { $p.Kill(); Record $name 'FAIL timeout'; throw 'Test timeout' }
    $p.WaitForExit()
    $text = Get-Content $stdout -Raw
    Write-Host $text
    if($p.ExitCode -eq 2) { Record $name 'NOT RUN - provider configuration or credential missing'; return }
    if($p.ExitCode -ne 0 -or $text -notmatch '((failed|failures)=0|Fails:\s*0)' -or $text -notmatch '(checks=|Checks:\s*)[1-9][0-9]*') {
        Record $name 'FAIL exit or summary'; Get-Content $stderr; throw "Test failed: $name"
    }
    Record $name 'PASS'
}
try {
    Push-Location $root
    if(!(Test-Path $umk)) {throw 'Required U++ toolchain missing'}
    $assembly = Get-Content (Join-Path $root 'github.var') -Raw
    if($assembly -notmatch 'upp_uidesigner/tests' -or $assembly -notmatch 'upp_Ui' -or $assembly -notmatch 'upp-18468/uppsrc') {throw 'Unexpected github assembly nests'}
    [pscustomobject]@{
        Designer=(& git rev-parse HEAD); Dependency=(& git -c safe.directory=E:/apps/github/upp_Ui -C E:/apps/github/upp_Ui rev-parse HEAD)
        Configuration=$Configuration; Scope=$Scope; Toolchain=$umk; Method='CLANGx64'; Assembly=$assembly
        SourceStatus=(& git status --short | Out-String); ToolchainHash=(Get-FileHash $umk -Algorithm SHA256).Hash
    } | ConvertTo-Json | Set-Content (Join-Path $evidence 'source.json')
    if($Scope -eq 'McpAttach') { Record 'MCP attach' 'NOT RUN - optional checkpoint B is not implemented'; throw 'MCP attachment is pending' }
    Test (Build 'AppChatTests' 'AppChatTests' $false)
    Test (Build 'AssistantDesignerTests' 'AssistantDesignerTests' $true)
    foreach($entry in @(@('RegressionTests',$true),@('ThemeDocumentTest',$false),@('ThemeStudioRoleTest',$true),@('ExportedThemeContractTest',$false))) {
        Test (Build $entry[0] $entry[0] $entry[1])
    }
    if($Broader) {
        foreach($entry in @(@('Tests',$true),@('FoundationTests',$false),@('ThemeAdapterCoverageTest',$false))) {
            Test (Build $entry[0] $entry[0] $entry[1])
        }
    }
    $app = Build 'UiDesigner/UiDesigner' 'UiDesigner' $true
    if($Live) {
        Test (Build 'AssistantLiveTest' 'AssistantLiveTest' $true)
        Record 'Human live Apply/Undo acceptance' 'NOT RUN - requires approval in Designer drawer'
    }
    else { Record 'Live provider acceptance' 'SKIP - offline run; no paid provider contacted' }
    if($Launch) {
        $p = Start-Process -FilePath $app -WorkingDirectory $root -PassThru
        Start-Sleep -Seconds 2
        if($p.HasExited) {throw 'Canonical Designer exited after launch'}
        $launchInfo = [pscustomobject]@{Path=$app;SHA256=(Get-FileHash $app -Algorithm SHA256).Hash;PID=$p.Id}
        $launchInfo | ConvertTo-Json | Set-Content (Join-Path $evidence 'launch.json')
        $launchInfo | Format-List | Out-Host
    }
    & git diff --check
    if($LASTEXITCODE -ne 0) {throw 'git diff --check failed'}
    Record 'Offline validation' 'PASS'
    Write-Host "Evidence: $evidence"
} catch {
    Record 'Validation' ('FAIL - '+$_.Exception.Message)
    Write-Host "Evidence: $evidence"
    exit 1
} finally { Pop-Location }
