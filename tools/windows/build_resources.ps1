# Generate Go-linker-compatible Windows resources from the Lakakey ICO.
# The Visual Studio cvtres.exe COFF format is not accepted by Go's .syso loader.
# Pin rsrc version for reproducible builds.
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
Set-Location $root
$icon = Join-Path $root 'assets/windows/lakakey.ico'
$resource = Join-Path $root 'desktop/windows/lakakey.syso'
if (-not (Test-Path $icon)) { throw "Official Lakakey .ico is missing: $icon" }

& go run 'github.com/akavel/rsrc@v0.10.2' '-ico' $icon '-o' $resource
if ($LASTEXITCODE -ne 0 -or -not (Test-Path $resource)) {
    throw 'Go-compatible icon resource generation failed'
}
if ((Get-Item $resource).Length -lt 1024) {
    throw 'Lakakey Windows resource is unexpectedly small'
}
Write-Host "Go-compatible Lakakey desktop icon resource: $resource"
