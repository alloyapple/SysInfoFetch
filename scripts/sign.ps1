<#
.SYNOPSIS
  Sign a Windows executable using SignTool and a PFX or certificate from the store.

.DESCRIPTION
  This script wraps Microsoft SignTool to sign an executable with SHA256 and an RFC3161
  timestamp. It supports signing with a local PFX file (password-protected) or using a
  certificate in the local machine store identified by thumbprint.

.PARAMETER ExePath
  Path to the executable to sign. Default: build\NeoFetchPro.exe

.PARAMETER PfxPath
  Path to a PFX file. If provided, the script will use the PFX to sign.

.PARAMETER PfxPassword
  Password for the PFX file. If omitted and PfxPath is provided, the script will prompt
  for a secure password interactively.

.PARAMETER CertThumbprint
  Thumbprint of a certificate in the CurrentUser or LocalMachine store to be used for signing.

.PARAMETER TimestampUrl
  RFC3161 timestamp server. Default: https://timestamp.digicert.com

.EXAMPLE
  .\sign.ps1 -ExePath build\NeoFetchPro.exe -PfxPath c:\keys\codesign.pfx

.EXAMPLE
  .\sign.ps1 -ExePath build\NeoFetchPro.exe -CertThumbprint "ABCDE123..."
#>

[CmdletBinding()]
param(
    [string]$ExePath = "build\\NeoFetchPro.exe",
    [string]$PfxPath,
    [SecureString]$PfxPassword,
    [string]$CertThumbprint,
    [string]$TimestampUrl = 'https://timestamp.digicert.com'
)

function Find-SignTool {
    # Try common Windows SDK locations first
    $candidates = @(
        "$env:ProgramFiles(x86)\\Windows Kits\\10\\bin",
        "$env:ProgramFiles(x86)\\Windows Kits\\8.1\\bin",
        "$env:ProgramFiles\\Windows Kits\\10\\bin"
    )
    foreach ($base in $candidates) {
        if (-not (Test-Path $base)) { continue }
        # try x64 subfolders
        Get-ChildItem -Path $base -Directory -ErrorAction SilentlyContinue | ForEach-Object {
            $path = Join-Path $_.FullName 'x64\\signtool.exe'
            if (Test-Path $path) { return $path }
            $path = Join-Path $_.FullName 'x86\\signtool.exe'
            if (Test-Path $path) { return $path }
        }
    }

    # fallback to PATH
    $where = (& where.exe signtool.exe 2>$null) -split '\r?\n' | Select-Object -First 1
    if ($where -and (Test-Path $where)) { return $where }
    return $null
}

if (-not (Test-Path $ExePath)) {
    Write-Error "Executable not found: $ExePath"; exit 2
}

$signtool = Find-SignTool
if (-not $signtool) {
    Write-Error "signtool.exe not found. Install Windows SDK or ensure signtool is in PATH."; exit 3
}

# Build sign command
$args = @()
$args += 'sign'
$args += '/fd' ; $args += 'SHA256'
$args += '/td' ; $args += 'SHA256'
$args += '/tr' ; $args += $TimestampUrl

if ($PfxPath) {
    if (-not (Test-Path $PfxPath)) { Write-Error "PFX not found: $PfxPath"; exit 4 }
    if (-not $PfxPassword) {
        $sec = Read-Host -Prompt 'Enter PFX password (hidden)' -AsSecureString
        $PfxPassword = $sec
    }
    # convert secure string to plain text for signtool
    $ptr = [Runtime.InteropServices.Marshal]::SecureStringToBSTR($PfxPassword)
    try { $plainPwd = [Runtime.InteropServices.Marshal]::PtrToStringBSTR($ptr) } finally { [Runtime.InteropServices.Marshal]::ZeroFreeBSTR($ptr) }

    $args += '/f' ; $args += (Resolve-Path $PfxPath).Path
    $args += '/p' ; $args += $plainPwd
} elseif ($CertThumbprint) {
    $args += '/sha1' ; $args += $CertThumbprint
} else {
    Write-Error "No signing identity provided. Use -PfxPath or -CertThumbprint."; exit 5
}

$args += (Resolve-Path $ExePath).Path

Write-Host "Using signtool: $signtool"
Write-Host "Signing: $ExePath"

$proc = Start-Process -FilePath $signtool -ArgumentList $args -NoNewWindow -Wait -PassThru
if ($proc.ExitCode -ne 0) { Write-Error "signtool failed with exit code $($proc.ExitCode)"; exit $proc.ExitCode }

Write-Host "Signing complete." -ForegroundColor Green
