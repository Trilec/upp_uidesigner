param([string]$Cli = "$PSScriptRoot/build/uidesigner_cli.exe")
$ErrorActionPreference = 'Stop'
$destination = Join-Path $PSScriptRoot 'skills/uidesigner-design/references/controls'
New-Item -ItemType Directory -Force -Path $destination | Out-Null
$catalog = (& $Cli list-controls | Out-String | ConvertFrom-Json)
if ($LASTEXITCODE -ne 0 -or !$catalog.ok) { throw 'Cannot read control catalogue' }
$utf8 = New-Object System.Text.UTF8Encoding($false)
[IO.File]::WriteAllText((Join-Path $destination 'index.json'), ($catalog.result | ConvertTo-Json -Depth 80), $utf8)
foreach ($control in $catalog.result) {
    $schema = (& $Cli schema $control.type | Out-String | ConvertFrom-Json)
    if ($LASTEXITCODE -ne 0 -or !$schema.ok) { throw "Cannot describe $($control.type)" }
    [IO.File]::WriteAllText((Join-Path $destination ($control.type + '.json')), ($schema.result | ConvertTo-Json -Depth 80 -Compress), $utf8)
}
Write-Host "Exported $($catalog.result.Count) schemas from the canonical catalogue."
