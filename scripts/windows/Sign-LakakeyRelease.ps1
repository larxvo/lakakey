#Requires -Version 5.1
<#
  Sign-LakakeyRelease.ps1
  Authenticode sign Windows release binaries with a publicly trusted PFX.

  Required environment variables:
    LAKAKEY_SIGN_PFX_BASE64   (base64-encoded PFX secret)
    LAKAKEY_SIGN_PFX_PASSWORD (PFX password secret)

  This script deliberately fails if no trusted code-signing certificate is present.
  It cannot manufacture a trusted publisher or bypass SmartScreen reputation.
  Copyright (c) 2026 Larx Vo — https://vovinhlac.com
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string[]]$Files
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ($PSVersionTable.PSEdition -eq 'Core' -and -not $IsWindows) {
    throw 'Authenticode Windows release signing must run on a Windows runner.'
}
if ([string]::IsNullOrWhiteSpace($env:LAKAKEY_SIGN_PFX_BASE64) -or
    [string]::IsNullOrWhiteSpace($env:LAKAKEY_SIGN_PFX_PASSWORD)) {
    throw 'Trusted code-signing certificate is not configured. No signed release will be created.'
}

$kit = Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Windows Kits\10\bin'
$tool = Get-ChildItem -Path $kit -Filter 'signtool.exe' -Recurse -File |
    Where-Object { $_.FullName -match '[\\/]x64[\\/]signtool.exe$' } |
    Sort-Object FullName -Descending |
    Select-Object -First 1
if (-not $tool) { throw 'Windows SDK x64 signtool.exe was not found.' }

$pfxPath = Join-Path $env:RUNNER_TEMP ('lakakey-sign-' + [guid]::NewGuid().ToString('N') + '.pfx')
try {
    [IO.File]::WriteAllBytes($pfxPath, [Convert]::FromBase64String($env:LAKAKEY_SIGN_PFX_BASE64))
    foreach ($item in $Files) {
        $file = (Resolve-Path -LiteralPath $item).Path
        # Timestamp so signatures can remain valid after the certificate expires.
        & $tool.FullName sign /f $pfxPath /p $env:LAKAKEY_SIGN_PFX_PASSWORD /fd SHA256 
            /tr 'http://timestamp.digicert.com' /td SHA256 $file
        if ($LASTEXITCODE -ne 0) { throw "signtool sign failed for $file" }

        $signature = Get-AuthenticodeSignature -LiteralPath $file
        if ($signature.Status -ne 'Valid') {
            throw "Invalid/untrusted Authenticode signature for $file: $($signature.Status)"
        }
        if (-not $signature.SignerCertificate) { throw "Missing signing certificate for $file" }
        $codeSigningEKU = $signature.SignerCertificate.EnhancedKeyUsageList |
            Where-Object { $_.ObjectId -eq '1.3.6.1.5.5.7.3.3' } |
            Select-Object -First 1
        if (-not $codeSigningEKU) { throw "Certificate is not valid for Code Signing: $file" }
        Write-Host "Verified Authenticode signature: $([IO.Path]::GetFileName($file))"
        Write-Host "Signer subject: $($signature.SignerCertificate.Subject)"
        Write-Host "SHA-256: $((Get-FileHash -LiteralPath $file -Algorithm SHA256).Hash)"
    }
}
finally {
    if (Test-Path -LiteralPath $pfxPath) {
        Remove-Item -LiteralPath $pfxPath -Force -ErrorAction SilentlyContinue
    }
}
