# Phase 0 Build Status

- Native Qt 6 / QML desktop source is present.
- A separate Windows service process and SCM entry point are present.
- Versioned local IPC protocol and desktop client are present.
- Windows build script and Inno Setup definition are present.
- Static source checks pass in the current environment.

**A real Windows build has not yet been completed.** This environment has no Qt SDK or Windows/MSVC toolchain, so no Phase 0 native EXE or installer has been produced or tested here. The previous browser-based binaries are not being represented as native outputs.

## Update — 2026-11-10

- The named-pipe worker shutdown path no longer depends on `QThread::nativeHandle()`; it uses a duplicated Win32 thread handle for synchronous-I/O cancellation.
- A GitHub Actions Windows build workflow is included at `.github/workflows/windows-build.yml`. It uses a Windows Server 2022 runner with Visual Studio 2022 and installs Qt 6.8.3, so Visual Studio does not need to be installed on the user's PC.
- See `docs/CLOUD_BUILD_FA.md` for the cloud build steps.
- The workflow has not been run yet; Windows compilation and runtime behavior remain unverified.
