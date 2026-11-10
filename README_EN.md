# WBSJ WE — Phase 0 (Native Desktop Foundation)

**Version:** 0.2.0-phase0  
**Goal:** establish the native Windows desktop shell, independent background service and local IPC channel.

> This package starts the Native rewrite and supersedes the browser-based 0.1.0 package. Phase 0 is not yet an operational NVR: no camera, live video or recording activity is simulated.

## Implemented in Phase 0

- Native Qt 6 / QML desktop application; it does not launch a browser.
- Separate `WBSJ_WE_RecordingService.exe` process.
- Windows Service Control Manager entry point named `WBSJWERecordingService`.
- Console mode for development and diagnostics (`--console`).
- Local IPC using QLocalServer / QLocalSocket and a versioned newline-delimited JSON protocol.
- Service health response, version, start time, configured camera count and active recorder count (both counts are genuinely zero in Phase 0).
- Service log under `%PROGRAMDATA%\WBSJ WE\logs`.
- UI actions to open the data folder, log folder and Windows Services console.
- Dark desktop shell and an EN/FA language toggle for the Phase 0 screen.
- PowerShell service install/uninstall scripts and an Inno Setup installer template.

## Not implemented yet

- ONVIF discovery and camera management.
- RTSP URI resolution from ONVIF media profiles and actual stream tests.
- Decode, Live View, audio and PTZ.
- Recording, segmentation, retention and playback.
- Windows DPAPI camera credential storage.
- AI, Cloud and P2P.

These items are future phases and are not represented as working features in this build.

## Build prerequisites

- Windows 10/11 x64
- Visual Studio 2022 with Desktop development with C++ and Windows SDK
- CMake 3.24+
- Qt 6.5+ with Core, Gui, Qml, Quick, QuickControls2 and Network, using a compatible MSVC kit
- Inno Setup 6 to build the installer

## Build on Windows

```powershell
$env:Qt6_DIR = 'C:\Qt\6.8.3\msvc2022_64\lib\cmake\Qt6'
.\scripts\build-windows.ps1
```

Then compile `packaging/windows/WBSJWE.iss` with Inno Setup Compiler. The installer output is placed in `dist`.

## Development run

Start the service in console mode:

```powershell
.\build-windows\Release\WBSJ_WE_RecordingService.exe --console
```

In a second terminal:

```powershell
.\build-windows\Release\WBSJ_WE.exe
```

To install it as a Windows service, run elevated PowerShell:

```powershell
.\scripts\install-service.ps1 -ServiceExe .\build-windows\Release\WBSJ_WE_RecordingService.exe
```

Remove it with:

```powershell
.\scripts\uninstall-service.ps1
```

## Phase 0 IPC protocol

Socket name: `WBSJ_WE_RecordingService_v1`  
Protocol version: `1`  
Framing: one JSON request/response per line, terminated by `\n`.

Status request:

```json
{"protocol":1,"command":"status"}
```

Only `status` and `ping` are allowed in Phase 0. There are no IPC commands to start/stop recording or modify configuration. On Windows the local status channel is accessible to local clients; it exposes health metadata only. Before adding administrative IPC commands, implement a dedicated named-pipe ACL, authentication and authorization.

## Data paths

Windows:

```text
%PROGRAMDATA%\WBSJ WE\
    logs\recording-service.log
```

## Build and validation limitation

The current build environment does not have Qt 6 or FFmpeg development libraries installed, so a real Qt build or Windows EXE build/test cannot be performed here. The Windows build script is prepared for a Windows host with Qt/MSVC. Until it is built and tested on Windows, this source package must not be treated as an installer-ready or operational NVR release.

## Copyright

© WBSJ WE — All Rights Reserved  
https://wbsj.ir
