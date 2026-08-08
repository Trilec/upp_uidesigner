$ErrorActionPreference = 'Stop'

$root = $PSScriptRoot

function Fail([string]$message) { throw "UiDesigner architecture guard: $message" }
function Require-Path([string]$relative) {
    $path = Join-Path $root $relative
    if(-not (Test-Path -LiteralPath $path)) { Fail "missing $relative" }
}
function Require-Text([string]$relative, [string]$pattern) {
    $path = Join-Path $root $relative
    if(-not (Select-String -LiteralPath $path -Pattern $pattern -Quiet)) {
        Fail "$relative does not contain required contract: $pattern"
    }
}
function Forbid-Text([string]$relative, [string]$pattern) {
    $path = Join-Path $root $relative
    if(Select-String -LiteralPath $path -Pattern $pattern -Quiet) {
        Fail "$relative contains forbidden dependency/mutation: $pattern"
    }
}
function Forbid-InTree([string]$relative, [string]$pattern) {
    $path = Join-Path $root $relative
    $match = Get-ChildItem -LiteralPath $path -Recurse -File -Include *.h,*.cpp,*.upp |
        Select-String -Pattern $pattern | Select-Object -First 1
    if($match) { Fail "$relative contains '$pattern' at $($match.Path):$($match.LineNumber)" }
}

$requiredPackages = @(
    'UiDesigner\Core',
    'UiDesigner\Commands',
    'UiDesigner\Catalog',
    'UiDesigner\Preview',
    'UiDesigner\CodeGen',
    'UiDesigner\ThemeCore',
    'UiDesigner\Theme',
    'UiDesigner\Services',
    'UiDesigner\CLI',
    'UiDesigner\MCP',
    'UiDesigner\UiDesigner',
    'tests\Tests',
    'tests\FoundationTests'
)
foreach($package in $requiredPackages) { Require-Path $package }

$requiredDocs = @(
    'docs\DRAG_DROP_DESIGN.md',
    'docs\BEHAVIOR_BINDING_DESIGN.md',
    'docs\EXPORT_CONTRACT.md',
    'docs\IMPLEMENTATION_STATUS.md'
)
foreach($doc in $requiredDocs) { Require-Path $doc }

$mainPath = Join-Path $root 'UiDesigner\UiDesigner\main.cpp'
$mainLines = @(Get-Content -LiteralPath $mainPath).Count
if($mainLines -gt 40) { Fail "application main.cpp is $mainLines lines; shell composition belongs outside main" }

foreach($headless in @(
    'UiDesigner\Core',
    'UiDesigner\Commands',
    'UiDesigner\Catalog',
    'UiDesigner\CodeGen',
    'UiDesigner\ThemeCore',
    'UiDesigner\Services',
    'UiDesigner\CLI',
    'UiDesigner\MCP'
)) {
    Forbid-InTree $headless '^\s*#include\s+<Ui/'
    Forbid-InTree $headless '^\s*#include\s+<CtrlLib/'
    Forbid-InTree $headless 'UiDesigner/Preview/'
    Forbid-InTree $headless 'Utilities/Designer/'
}

Forbid-InTree 'UiDesigner' 'Utilities/Designer/main\.cpp'
Forbid-Text 'UiDesigner\UiDesigner\UiDesignerWindow.cpp' 'Document\(\)\.(Set|Add|Remove|Move|Replace)'
Forbid-Text 'UiDesigner\UiDesigner\UiDesignerWindow.cpp' 'GetNodes\(\)\.(Add|Remove|Set)'

Require-Text 'UiDesigner\Catalog\UiDesignerBuiltins.cpp' 'type_id\s*=\s*"Spacer"'
Require-Text 'UiDesigner\Catalog\UiDesignerBuiltins.cpp' 'UiDesignerNodeSemanticItem'
Require-Text 'UiDesigner\Theme\UiDesignerThemeAdapter.h' 'class UiDesignerThemeAdapter'
Require-Text 'UiDesigner\Theme\UiDesignerThemeAdapter.cpp' 'ButtonThemeAdapter'
Require-Text 'UiDesigner\Theme\UiDesignerThemeAdapter.cpp' 'TreeThemeAdapter'
Require-Text 'UiDesigner\Theme\UiDesignerThemeAdapter.cpp' 'ListThemeAdapter'
Require-Text 'UiDesigner\Theme\UiDesignerThemeAdapter.cpp' 'MenuThemeAdapter'
Require-Text 'UiDesigner\Core\UiDesignerTypes.h' 'UiDesignerActionBinding'
Require-Text 'UiDesigner\Services\UiDesignerDrop.cpp' 'UiDesignerDropService::PlanAdd'
Require-Text 'UiDesigner\Services\UiDesignerDrop.cpp' 'UiDesignerDropService::PlanMove'
Require-Text 'UiDesigner\Services\UiDesignerExport.cpp' 'UiDesignerExportProfile::ComponentOnly'
Require-Text 'UiDesigner\CodeGen\UiDesignerCodeGen.cpp' 'UiDesignerChildAdapterEntry'
Require-Text 'UiDesigner\CodeGen\UiDesignerCodeGen.cpp' 'RestorePublished'
Require-Text 'tests\FoundationTests\main.cpp' 'legacy root Spacer imports'
Require-Text 'tests\FoundationTests\main.cpp' 'package filename is independent from class name'
Require-Text 'UiDesigner\CLI\main.cpp' 'behavior-set'
Require-Text 'UiDesigner\Services\UiDesignerAutomation.cpp' 'uidesigner_apply_drop'
Require-Text 'UiDesigner\Services\UiDesignerAutomation.cpp' 'uidesigner_set_behavior'
Require-Text 'UiDesigner\Services\UiDesignerAutomation.cpp' 'uidesigner_export'
Require-Text 'UiDesigner\Catalog\UiDesignerCatalog.h' 'theme_adapter_id'
Forbid-Text 'UiDesigner\Catalog\UiDesignerCatalog.cpp' 'button_style_field'
Forbid-Text 'UiDesigner\Preview\UiDesignerPreview.cpp' 'button_style_field'
Forbid-Text 'UiDesigner\CodeGen\UiDesignerCodeGen.cpp' 'button_style_field'
Forbid-Text 'UiDesigner\Services\UiDesignerSession.cpp' 'button_style_field'

foreach($obsolete in @(
    'UiDesigner\.integrated',
    'UiDesigner\.bundle',
    'UiDesigner\InstallIntegratedSource.ps1',
    'UiDesigner\InstallSource.ps1'
)) {
    if(Test-Path -LiteralPath (Join-Path $root $obsolete)) {
        Fail "obsolete source-delivery transport remains: $obsolete"
    }
}

Write-Host 'UiDesigner architecture guard: PASS'
