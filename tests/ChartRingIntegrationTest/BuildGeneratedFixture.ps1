param(
    [string]$UmkPath = 'E:\upp-18468\umk.exe',
    [string]$Assembly = 'github',
    [string]$Config = 'CLANGx64',
    [string]$OutputRoot = 'E:\apps\github\upp_uidesigner\build',
    [switch]$DebugBuild
)

$ErrorActionPreference = 'Stop'
$repoRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$generatedRoot = Join-Path $PSScriptRoot '.generated-smoke'
$flags = if ($DebugBuild) { '-b' } else { '-br' }
$flavour = if ($DebugBuild) { 'Debug' } else { 'Release' }
$logs = Join-Path $OutputRoot ('UID-CHARTRING-01-' + $flavour + '-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))
$fixtureExe = Join-Path $OutputRoot ('UiDesignerChartRingIntegrationTest' + $flavour + '.exe')
$foreignCwd = Join-Path $logs 'foreign-cwd'

function Build-Checked([string]$package, [string]$exe, [string]$label) {
    if (Test-Path -LiteralPath $exe) { Remove-Item -LiteralPath $exe -Force }
    $stdout = Join-Path $logs ($label + '.build.stdout.log')
    $stderr = Join-Path $logs ($label + '.build.stderr.log')
    $arguments = @(('"' + $Assembly + '"'), ('"' + $package + '"'),
                   ('"' + $Config + '"'), $flags, '+GUI', ('"' + $exe + '"'))
    $process = Start-Process -FilePath $UmkPath -ArgumentList $arguments `
        -WorkingDirectory $repoRoot -Wait -PassThru `
        -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    $code = $process.ExitCode
    Get-Content -LiteralPath $stdout
    Get-Content -LiteralPath $stderr
    if ($null -eq $code -or $code -ne 0 -or -not (Test-Path -LiteralPath $exe -PathType Leaf)) {
        throw "$label build failed (exit $code)"
    }
}

function Run-Checked([string]$exe, [string]$label, [string[]]$arguments = @()) {
    $stdout = Join-Path $logs ($label + '.stdout.log')
    $stderr = Join-Path $logs ($label + '.stderr.log')
    $start = @{
        FilePath = $exe
        WorkingDirectory = $foreignCwd
        PassThru = $true
        RedirectStandardOutput = $stdout
        RedirectStandardError = $stderr
    }
    if ($arguments.Count) { $start.ArgumentList = $arguments }
    $process = Start-Process @start
    $null = $process.Handle
    if (-not $process.WaitForExit(120000)) {
        Stop-Process -Id $process.Id -Force
        throw "$label timed out; logs retained at $logs"
    }
    $process.WaitForExit()
    $code = $process.ExitCode
    Get-Content -LiteralPath $stdout
    Get-Content -LiteralPath $stderr
    if ($null -eq $code -or $code -ne 0) { throw "$label failed (exit $code)" }
}

if (-not (Test-Path -LiteralPath $UmkPath -PathType Leaf)) { throw "Missing umk: $UmkPath" }
New-Item -ItemType Directory -Force -Path $OutputRoot, $logs, $foreignCwd | Out-Null
# Only this script's disposable fixture subtree is replaced. Preserve it on failure.
if (Test-Path -LiteralPath $generatedRoot) { Remove-Item -LiteralPath $generatedRoot -Recurse -Force }
New-Item -ItemType Directory -Path $generatedRoot | Out-Null

Push-Location -LiteralPath $repoRoot
try {
    Build-Checked 'ChartRingIntegrationTest' $fixtureExe 'integration'
    Run-Checked $fixtureExe 'integration'
    if ((Get-Content -LiteralPath (Join-Path $logs 'integration.stdout.log') -Raw) -notmatch
            'CHARTRING_INTEGRATION checks=\d+ failed=0') {
        throw 'Integration executable did not produce the expected summary'
    }
    Run-Checked $fixtureExe 'export' @('--export-fixture', ('"' + $generatedRoot + '"'))

    $complete = Join-Path $generatedRoot 'ChartRingFixture'
    foreach ($file in @('ChartRingFixture.upp', 'ChartRingWindow.generated.h',
                       'ChartRingWindow.generated.cpp', 'ChartRingWindow.h',
                       'ChartRingWindow.cpp', 'main.cpp', 'design.json', 'theme.json')) {
        if (-not (Test-Path -LiteralPath (Join-Path $complete $file) -PathType Leaf)) {
            throw "Missing generated artifact: $file"
        }
    }
    # Prove independence even when optional authoring metadata is absent.
    Remove-Item -LiteralPath (Join-Path $complete 'theme.json') -Force
    $componentTheme = Join-Path $generatedRoot 'Component\theme.json'
    if (Test-Path -LiteralPath $componentTheme) { Remove-Item -LiteralPath $componentTheme -Force }

    $app = Join-Path $OutputRoot ('ChartRingFixture' + $flavour + '.exe')
    Build-Checked 'ChartRingIntegrationTest/.generated-smoke/ChartRingFixture' $app 'complete-app'
    foreach ($package in @('ChartRingVerify', 'Component')) {
        $exe = Join-Path $OutputRoot ($package + $flavour + '.exe')
        Build-Checked ('ChartRingIntegrationTest/.generated-smoke/' + $package) $exe $package
        Run-Checked $exe $package
        if ((Get-Content -LiteralPath (Join-Path $logs ($package + '.stdout.log')) -Raw) -notmatch
                'CHARTRING_GENERATED_RUNTIME failed=0') {
            throw "$package did not produce the expected runtime proof"
        }
    }
    Write-Host "PASS: ChartRing $flavour integration, complete app build and complete/component runtime proof."
    Write-Host "Evidence: $logs"
    Remove-Item -LiteralPath $generatedRoot -Recurse -Force
}
catch {
    Write-Host "Fixture retained: $generatedRoot"
    Write-Host "Evidence: $logs"
    throw
}
finally { Pop-Location }
