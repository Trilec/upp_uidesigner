param(
    [string]$UmkPath = 'E:\upp-18468\umk.exe',
    [string]$Assembly = 'github',
    [string]$Config = 'CLANGx64',
    [string]$OutputRoot = 'E:\apps\github\upp_uidesigner\build'
)

$ErrorActionPreference = 'Stop'
$repoRoot = $PSScriptRoot

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

Invoke-Checked 'PropertyEditorCoreProbe' { & $probe }
Invoke-Checked 'PropertyEditorTests' { & $propertyTests }
Invoke-Checked 'PropertyEditorOverrideCommitTest' { & $propertyOverride }
Invoke-Checked 'UiThemeStructureContractTest' { & $uiThemeStructure }
Invoke-Checked 'UiDesignerTests' { & $designerTests }
Invoke-Checked 'UiDesignerRegressionTests' { & $regressionTests }
Invoke-Checked 'UiDesignerFoundationTests' { & $foundationTests }
Invoke-Checked 'ThemeStructureOwnershipTest' { & $themeStructure }
Invoke-Checked 'ThemeAdapterCoverageTest' { & $themeCoverage }
Invoke-Checked 'ThemeDarkIntegrationTest' { & $themeDark }
Invoke-Checked 'ThemeBuilderContractTest' { & $themeBuilder }
Invoke-Checked 'CurrentUiIntegrationTest' { & $currentUi }
Invoke-Checked 'DesignerClosureCatalogTest' { & $closureCatalog }
Invoke-Checked 'ExportedThemeContractTest' { & $exportedTheme }
Invoke-Checked 'UiSplitterCatalogTest' { & $splitterCatalog }

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

Write-Host "`nUiDesigner supervisor validation sequence completed."
Write-Host "GUI executable: $app"
Write-Host 'Interactive design, drag/drop and dialog validation still requires a visible desktop session.'
