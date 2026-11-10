# Windows service lifecycle

Service internal name: `WBSJWERecordingService`  
Display name: `WBSJ WE Recording Service`  
Startup: Automatic  
Executable: `WBSJ_WE_RecordingService.exe`

The service is a separate process from the desktop application. It can start at boot under the Windows Service Control Manager, before a user signs in. Closing the desktop window does not stop the service.

## Install

Run PowerShell as Administrator:

```powershell
.\scripts\install-service.ps1 -ServiceExe 'C:\Program Files\WBSJ WE\WBSJ_WE_RecordingService.exe'
```

## Inspect

```powershell
Get-Service WBSJWERecordingService
services.msc
```

## Remove

```powershell
.\scripts\uninstall-service.ps1
```

## Logs

`%PROGRAMDATA%\WBSJ WE\logs\recording-service.log`

Phase 0 service responsibilities are limited to process lifecycle, local IPC status and logging. The recording pipeline is a later phase.
