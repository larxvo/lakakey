# Compile a Windows ICO resource into Go-linkable COFF .syso.
# Requires Windows SDK rc.exe and Visual Studio cvtres.exe (on windows-2022).
$ErrorActionPreference = 'Stop'
$root = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path
Set-Location $root
$sdkRoot = Join-Path ${env:ProgramFiles(x86)} 'Windows Kits/10/bin'
$rc = Get-ChildItem -Path $sdkRoot -Filter rc.exe -Recurse -File |
  Where-Object { $_.FullName -match '\\x64\\rc\.exe$' } |
  Sort-Object FullName -Descending | Select-Object -First 1
if (-not $rc) { throw 'Windows SDK resource compiler (x64 rc.exe) is required' }

$vswhere = Join-Path ${env:ProgramFiles(x86)} 'Microsoft Visual Studio/Installer/vswhere.exe'
if (-not (Test-Path $vswhere)) { throw 'vswhere.exe missing' }
$install = (& $vswhere -latest -products '*' -property installationPath).Trim()
if (-not $install) { throw 'Visual Studio 2022 was not found' }
$versionFile = Join-Path $install 'VC/Auxiliary/Build/Microsoft.VCToolsVersion.default.txt'
$vcVersion = (Get-Content -Raw $versionFile).Trim()
$cvtres = Join-Path $install "VC/Tools/MSVC/$vcVersion/bin/Hostx64/x64/cvtres.exe"
if (-not (Test-Path $cvtres)) { throw "cvtres.exe missing: $cvtres" }

$input = Join-Path $root 'assets/windows/lakakey-app.rc'
$res = Join-Path $root 'desktop/windows/lakakey.res'
$output = Join-Path $root 'desktop/windows/lakakey.syso'

& $rc.FullName '/nologo' ("/fo$res") $input
if ($LASTEXITCODE -ne 0 -or -not (Test-Path $res)) { throw 'Icon RC compilation failed' }
& $cvtres '/nologo' '/machine:x64' ("/out:$output") $res
if ($LASTEXITCODE -ne 0 -or -not (Test-Path $output)) { throw 'Icon COFF conversion failed' }
Remove-Item -LiteralPath $res -Force
Write-Host "Embedded Lakakey app icon resource: $output"
