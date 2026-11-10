[CmdletBinding()]
param(
    [Parameter(Mandatory=$true)][string]$ServiceExe,
    [switch]$Start = $true
)
$ErrorActionPreference = 'Stop'
$identity = [Security.Principal.WindowsIdentity]::GetCurrent()
$principal = [Security.Principal.WindowsPrincipal]::new($identity)
if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
    throw 'Run this script from an elevated PowerShell session.'
}
$resolved = (Resolve-Path $ServiceExe).Path
$serviceName = 'WBSJWERecordingService'
$existing = Get-Service -Name $serviceName -ErrorAction SilentlyContinue
if ($existing) {
    if ($existing.Status -ne 'Stopped') { Stop-Service -Name $serviceName -Force }
    & sc.exe delete $serviceName | Out-Null
    Start-Sleep -Seconds 2
}
$binPath = '"' + $resolved + '"'
& sc.exe create $serviceName binPath= $binPath start= auto DisplayName= 'WBSJ WE Recording Service' | Out-Null
if ($LASTEXITCODE -ne 0) { throw "sc.exe create failed: $LASTEXITCODE" }
& sc.exe description $serviceName 'Background recording and device service for WBSJ WE.' | Out-Null
& sc.exe failure $serviceName reset= 86400 actions= restart/5000/restart/15000/restart/30000 | Out-Null
if ($Start) { Start-Service -Name $serviceName }
Get-Service -Name $serviceName | Format-List Name,DisplayName,Status,StartType
