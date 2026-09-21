param(
    [string]$UmkPath = 'E:\upp-18468\umk.exe',
    [string]$Assembly = 'github',
    [string]$Config = 'CLANGx64',
    [string]$OutputRoot = 'E:\apps\github\upp_uidesigner\build'
)

$ErrorActionPreference = 'Stop'
$repoRoot = $PSScriptRoot
$runLogs = Join-Path $OutputRoot ('supervisor-' + (Get-Date -Format 'yyyyMMdd-HHmmss'))

function Run-Test([string]$executable) {
    $name = [IO.Path]::GetFileNameWithoutExtension($executable)
    $stdout = Join-Path $runLogs ($name + '.stdout.log')
    $stderr = Join-Path $runLogs ($name + '.stderr.log')
    $process = Start-Process -FilePath $executable -WorkingDirectory $repoRoot `
        -WindowStyle Hidden -PassThru -RedirectStandardOutput $stdout -RedirectStandardError $stderr
    $null = $process.Handle
    $process.WaitForExit()
    Get-Content -LiteralPath $stdout
    Get-Content -LiteralPath $stderr
    Write-Host "$name exit: $($process.ExitCode)"
    if($null -eq $process.ExitCode -or $process.ExitCode -ne 0) {
        throw "$name failed with exit code $($process.ExitCode); evidence: $runLogs"
    }
}

function Invoke-Checked([string]$label, [scriptblock]$command) {
    Write-Host "`n== $label =="
    & $command
    if($LASTEXITCODE -ne 0) {
        throw "$label failed with exit code $LASTEXITCODE"
    }
}

function Build-Package([string]$package, [string]$output, [bool]$gui = $false) {
    Invoke-Checked "Build $package" {
        if($gui) {
            & $UmkPath $Assembly $package $Config '-br' '+GUI' $output
        }
        else {
            & $UmkPath $Assembly $package $Config '-br' $output
        }
    }
}

if(-not (Test-Path -LiteralPath $UmkPath -PathType Leaf)) {
    throw "umk was not found at $UmkPath"
}
New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null
New-Item -ItemType Directory -Path $runLogs -Force | Out-Null

Invoke-Checked 'Architecture guard' {
    powershell -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'ValidateArchitecture.ps1')
}

$probe = Join-Path $OutputRoot 'PropertyEditorCoreProbe.exe'
$propertyTests = Join-Path $OutputRoot 'PropertyEditorTests.exe'
$propertyOverride = Join-Path $OutputRoot 'PropertyEditorOverrideCommitTest.exe'
$uiThemeStructure = Join-Path $OutputRoot 'UiThemeStructureContractTest.exe'
$designerTests = Join-Path $OutputRoot 'UiDesignerTests.exe'
$regressionTests = Join-Path $OutputRoot 'UiDesignerRegressionTests.exe'
$foundationTests = Join-Path $OutputRoot 'UiDesignerFoundationTests.exe'
$themeStructure = Join-Path $OutputRoot 'UiDesignerThemeStructureOwnershipTest.exe'
$themeCoverage = Join-Path $OutputRoot 'UiDesignerThemeAdapterCoverageTest.exe'
$themeDark = Join-Path $OutputRoot 'UiDesignerThemeDarkIntegrationTest.exe'
$themeBuilder = Join-Path $OutputRoot 'UiDesignerThemeBuilderContractTest.exe'
$currentUi = Join-Path $OutputRoot 'UiDesignerCurrentUiIntegrationTest.exe'
$closureCatalog = Join-Path $OutputRoot 'UiDesignerClosureCatalogTest.exe'
$exportedTheme = Join-Path $OutputRoot 'UiDesignerExportedThemeContractTest.exe'
$splitterCatalog = Join-Path $OutputRoot 'UiDesignerUiSplitterCatalogTest.exe'
$cli = Join-Path $OutputRoot 'uidesigner_cli.exe'
$mcp = Join-Path $OutputRoot 'uidesigner_mcp.exe'
$app = Join-Path $OutputRoot 'UiDesigner.exe'

Build-Package 'Utilities/PropertyEditorCoreProbe' $probe
Build-Package 'Utilities/PropertyEditorTests' $propertyTests $true
Build-Package 'Utilities/PropertyEditorOverrideCommitTest' $propertyOverride
Build-Package 'Utilities/UiThemeStructureContractTest' $uiThemeStructure
Build-Package 'Tests' $designerTests $true
Build-Package 'RegressionTests' $regressionTests $true
Build-Package 'FoundationTests' $foundationTests
Build-Package 'ThemeStructureOwnershipTest' $themeStructure
Build-Package 'ThemeAdapterCoverageTest' $themeCoverage
Build-Package 'ThemeDarkIntegrationTest' $themeDark
Build-Package 'ThemeBuilderContractTest' $themeBuilder
Build-Package 'CurrentUiIntegrationTest' $currentUi
Build-Package 'DesignerClosureCatalogTest' $closureCatalog
Build-Package 'ExportedThemeContractTest' $exportedTheme
Build-Package 'UiSplitterCatalogTest' $splitterCatalog
Build-Package 'UiDesigner/CLI' $cli
Build-Package 'UiDesigner/MCP' $mcp
Build-Package 'UiDesigner/UiDesigner' $app $true

Run-Test $probe
Run-Test $propertyTests
Run-Test $propertyOverride
Run-Test $uiThemeStructure
Run-Test $designerTests
Run-Test $regressionTests
Run-Test $foundationTests
Run-Test $themeStructure
Run-Test $themeCoverage
Run-Test $themeDark
Run-Test $themeBuilder
Run-Test $currentUi
Run-Test $closureCatalog
Run-Test $exportedTheme
Run-Test $splitterCatalog

Invoke-Checked 'CLI list-controls' { & $cli 'list-controls' 'spacer' }
Invoke-Checked 'CLI schema Spacer' { & $cli 'schema' 'Spacer' }

Invoke-Checked 'MCP newline and Content-Length smoke' {
    powershell -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'UiDesigner\MCP\Smoke.ps1') -Executable $mcp
}

Invoke-Checked 'Generated package build smoke' {
    powershell -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'tests\FoundationTests\BuildGeneratedFixture.ps1') `
        -UmkPath $UmkPath -Assembly $Assembly -Config $Config -OutputRoot $OutputRoot
}

Invoke-Checked 'Creation preset generated-package builds' {
    powershell -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'tests\PresetExportTests\BuildPresetFixtures.ps1') `
        -UmkPath $UmkPath -Assembly $Assembly -Config $Config -OutputRoot $OutputRoot
}

Invoke-Checked 'ChartRing integration and generated runtime proof' {
    powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'tests\ChartRingIntegrationTest\BuildGeneratedFixture.ps1') `
        -UmkPath $UmkPath -Assembly $Assembly -Config $Config -OutputRoot $OutputRoot
}

Invoke-Checked 'DateTime integration and generated runtime proof' {
    powershell -NoProfile -ExecutionPolicy Bypass -File (Join-Path $PSScriptRoot 'tests\DateTimeIntegrationTest\BuildGeneratedFixture.ps1') `
        -UmkPath $UmkPath -Assembly $Assembly -Config $Config -OutputRoot $OutputRoot
}

Write-Host "`nUiDesigner supervisor validation sequence completed."
Write-Host "GUI executable: $app"
Write-Host 'Interactive design, drag/drop and dialog validation still requires a visible desktop session.'
