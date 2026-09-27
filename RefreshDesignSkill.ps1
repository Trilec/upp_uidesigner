param([string]$Cli = "$PSScriptRoot/build/uidesigner_cli.exe")
$ErrorActionPreference = 'Stop'
$destination = Join-Path $PSScriptRoot 'skills/uidesigner-design/references/controls'
New-Item -ItemType Directory -Force -Path $destination | Out-Null
$catalog = (& $Cli list-controls | Out-String | ConvertFrom-Json)
if ($LASTEXITCODE -ne 0 -or !$catalog.ok) { throw 'Cannot read control catalogue' }
$utf8 = New-Object System.Text.UTF8Encoding($false)
function Write-Schema([string]$path, [string]$content) {
    # Replace the directory entry; readers may have the prior schema memory-mapped.
    $temporarySchema = $path + '.tmp'
    [IO.File]::WriteAllText($temporarySchema, $content, $utf8)
    Move-Item -LiteralPath $temporarySchema -Destination $path -Force
}
Write-Schema (Join-Path $destination 'index.json') ($catalog.result | ConvertTo-Json -Depth 80)
foreach ($control in $catalog.result) {
    $schema = (& $Cli schema $control.type | Out-String | ConvertFrom-Json)
    if ($LASTEXITCODE -ne 0 -or !$schema.ok) { throw "Cannot describe $($control.type)" }
    Write-Schema (Join-Path $destination ($control.type + '.json')) ($schema.result | ConvertTo-Json -Depth 80 -Compress)
}
Write-Host "Exported $($catalog.result.Count) schemas from the canonical catalogue."
