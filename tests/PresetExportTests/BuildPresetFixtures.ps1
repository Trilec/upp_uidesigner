param(
    [string]$UmkPath = 'E:\upp-18468\umk.exe',
    [string]$Assembly = 'github',
    [string]$Config = 'CLANGx64',
    [string]$OutputRoot = 'E:\apps\github\upp_uidesigner\build'
)

$ErrorActionPreference = 'Stop'
$fixtureExe = Join-Path $OutputRoot 'UiDesignerPresetExportTests.exe'
$generatedRoot = Join-Path $PSScriptRoot '.generated-presets'

function Invoke-Checked([string]$label, [scriptblock]$command) {
    Write-Host "== $label =="
    & $command
    if($LASTEXITCODE -ne 0) {
        throw "$label failed with exit code $LASTEXITCODE"
    }
}

if(-not (Test-Path -LiteralPath $UmkPath -PathType Leaf)) {
    throw "umk was not found at $UmkPath"
}

Remove-Item -LiteralPath $generatedRoot -Recurse -Force -ErrorAction SilentlyContinue
New-Item -ItemType Directory -Path $generatedRoot -Force | Out-Null
New-Item -ItemType Directory -Path $OutputRoot -Force | Out-Null

try {
    Invoke-Checked 'Build PresetExportTests' {
        & $UmkPath $Assembly 'PresetExportTests' $Config '-br' $fixtureExe
    }
    Invoke-Checked 'Export all creation and catalog preset fixtures' {
        & $fixtureExe '--export-all' $generatedRoot
    }

    $packages = @(Get-ChildItem -LiteralPath $generatedRoot -Directory | Sort-Object Name)
    if($packages.Count -lt 4) {
        throw "Expected creation plus catalog preset packages, found $($packages.Count)"
    }

    foreach($packageDir in $packages) {
        $package = $packageDir.Name
        $uppFile = Join-Path $packageDir.FullName ($package + '.upp')
        if(-not (Test-Path -LiteralPath $uppFile -PathType Leaf)) {
            throw "Generated package is missing its .upp file: $uppFile"
        }
        $relative = "PresetExportTests/.generated-presets/$package"
        $output = Join-Path $OutputRoot ($package + '.exe')
        Invoke-Checked "Build generated preset $package" {
            & $UmkPath $Assembly $relative $Config '-br' '+GUI' $output
        }
        if(-not (Test-Path -LiteralPath $output -PathType Leaf)) {
            throw "Generated preset executable was not produced: $output"
        }
    }

    Write-Host "All $($packages.Count) generated creation/catalog preset packages built successfully."
    Remove-Item -LiteralPath $generatedRoot -Recurse -Force
}
catch {
    Write-Host "Generated preset fixtures retained for diagnosis: $generatedRoot"
    throw
}
