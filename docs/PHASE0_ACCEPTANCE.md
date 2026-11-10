# Phase 0 — Acceptance and verification plan

## Scope

Phase 0 is limited to the native desktop process, the independent Windows service process, local status IPC, basic service logging, and packaging scaffolding.

## Acceptance criteria

| ID | Criterion | Evidence / test |
|---|---|---|
| P0-01 | Desktop is a native Qt window and does not launch a browser | Run `WBSJ_WE.exe`; inspect process tree and QML application |
| P0-02 | Service is a separate executable | Confirm `WBSJ_WE.exe` and `WBSJ_WE_RecordingService.exe` are separate processes |
| P0-03 | Service can run interactively for diagnostics | Start service executable with `--console` |
| P0-04 | Service registers with Windows SCM | Install with `install-service.ps1`; verify `Get-Service WBSJWERecordingService` |
| P0-05 | Automatic startup is configured | Verify service startup type is Automatic and reboot a test VM |
| P0-06 | Desktop receives live service status via local IPC | Open desktop while service is running; verify Online state and version |
| P0-07 | Desktop shows service offline when unavailable | Stop service; verify Offline state and reconnect behavior |
| P0-08 | Service logs are written under ProgramData | Inspect `recording-service.log` |
| P0-09 | UI actions open logs/data folders and Services MMC | Click each quick action |
| P0-10 | No camera/recording activity is fabricated | Fresh install must show 0 cameras and 0 recorders |

## Not accepted as Phase 0 functionality

ONVIF, RTSP, video decode, Live View, audio, PTZ, recording, playback, retention, AI, Cloud and P2P are not implemented in this phase.

## Required Windows validation before release

1. Clean Windows 10 x64 VM with current updates.
2. Clean Windows 11 x64 VM with current updates.
3. Install as Administrator; verify service registration and automatic startup.
4. Reboot; verify service starts before desktop login.
5. Launch desktop as a standard user; verify IPC health state.
6. Stop/start service through `services.msc`; verify state transitions.
7. Uninstall; verify service removed and application binaries removed.
8. Confirm data and logs are retained by default during uninstall; do not delete recordings automatically.

## Current execution status

Source and build scripts are prepared. The current authoring environment has no Qt 6 SDK or Windows/MSVC toolchain, so the Qt targets and Windows installer have not been compiled or run here. All acceptance criteria therefore remain pending Windows execution.
