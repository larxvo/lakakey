#Requires -Version 5.1
<#
  Lakakey Authenticode signer — hardware-backed/Windows certificate store.
  Copyright (c) 2026 Larx Vo · https://vovinhlac.com

  Do NOT put private keys, PFX files or token PINs in the repository.
  Use an identity-verified code-signing certificate with a non-exportable key.
  This script refuses unsigned or untrusted release candidates.
#>
[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [string[]]$Files,
    [string]$Thumbprint = $env:LAKAKEY_SIGN_CERT_THUMBPRINT,
    [string]$ExpectedSubject = $env:LAKAKEY_SIGN_CERT_SUBJECT,
    [string]$TimestampUrl = 'http://timestamp.digicert.com'
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

if ([Environment]::OSVersion.Platform -ne [PlatformID]::Win32NT) {
    throw 'This Authenticode signing script requires Windows.'
}
if ([string]::IsNullOrWhiteSpace($Thumbprint) -or
    [string]::IsNullOrWhiteSpace($ExpectedSubject)) {
    throw 'Set a trusted code-signing certificate thumbprint and the exact legally verified certificate Subject.'
}

$thumb = ($Thumbprint -replace '[^0-9A-Fa-f]', '').ToUpperInvariant()
if ($thumb -notmatch '^(?:[A-F0-9]{40}|[A-F0-9]{64})$') {
    throw 'Invalid certificate thumbprint format.'
}

$cert = Get-ChildItem Cert:\CurrentUser\My |
    Where-Object { ($_.Thumbprint -replace '\s','').ToUpperInvariant() -eq $thumb } |
    Select-Object -First 1
if ($null -eq $cert) {
    throw 'Signing certificate not found in CurrentUser\My. Hardware token/HSM or provider integration may be unavailable.'
}
if (-not $cert.HasPrivateKey) { throw 'Certificate has no accessible private key.' }
if ($cert.Subject -cne $ExpectedSubject) {
    throw 'Signer Subject differs from the legally verified subject configured for this release.'
}
if ($cert.Subject -eq $cert.Issuer) { throw 'Self-signed certificates are not valid for trusted public Lakakey releases.' }
if ($cert.NotBefore -gt (Get-Date) -or $cert.NotAfter -le (Get-Date)) {
    throw 'Signing certificate is not currently valid.'
}
$hasCodeSigning = $false
foreach ($extension in $cert.Extensions) {
    if ($extension.Oid.Value -eq '2.5.29.37') {
        $eku = [System.Security.Cryptography.X509Certificates.X509EnhancedKeyUsageExtension]$extension
        foreach ($usage in $eku.EnhancedKeyUsages) {
            if ($usage.Value -eq '1.3.6.1.5.5.7.3.3') { $hasCodeSigning = $true }
        }
    }
}
if (-not $hasCodeSigning) { throw 'Certificate lacks the Code Signing extended key usage.' }

$chain = [System.Security.Cryptography.X509Certificates.X509Chain]::new()
$chain.ChainPolicy.RevocationMode = [System.Security.Cryptography.X509Certificates.X509RevocationMode]::Online
$chain.ChainPolicy.RevocationFlag = [System.Security.Cryptography.X509Certificates.X509RevocationFlag]::EntireChain
if (-not $chain.Build($cert)) {
    $details = ($chain.ChainStatus | ForEach-Object { $_.Status.ToString() }) -join ', '
    throw "Publisher certificate trust/revocation validation failed: $details"
}

$kitRoot = Join-Path ([Environment]::GetFolderPath('ProgramFilesX86')) 'Windows Kits\10\bin'
$signTool = Get-ChildItem -Path $kitRoot -Filter signtool.exe -File -Recurse |
    Where-Object { $_.FullName -match '[\\/]x64[\\/]signtool.exe$' } |
    Sort-Object FullName -Descending | Select-Object -First 1
if ($null -eq $signTool) { throw 'Windows SDK x64 signtool.exe is required.' }

foreach ($item in $Files) {
    $path = (Resolve-Path -LiteralPath $item -ErrorAction Stop).Path
    & $signTool.FullName sign /sha1 $thumb /s My /fd SHA256 /tr $TimestampUrl /td SHA256 $path
    if ($LASTEXITCODE -ne 0) { throw "Authenticode signing failed: $path" }

    $signature = Get-AuthenticodeSignature -LiteralPath $path
    if ($signature.Status -ne 'Valid') { throw "Invalid Authenticode signature: $path ($($signature.Status))" }
    if ($null -eq $signature.SignerCertificate -or
        $signature.SignerCertificate.Thumbprint.ToUpperInvariant() -ne $thumb) {
        throw "Wrong certificate signed $path"
    }
    & $signTool.FullName verify /pa /tw /v $path
    if ($LASTEXITCODE -ne 0) { throw "Authenticode chain/timestamp verification failed: $path" }

    $hash = (Get-FileHash -LiteralPath $path -Algorithm SHA256).Hash.ToLowerInvariant()
    Write-Host "Verified: $([IO.Path]::GetFileName($path))"
    Write-Host "Verified legal publisher subject: $ExpectedSubject"
    Write-Host "SHA-256: $hash"
}
