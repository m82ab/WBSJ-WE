# Dependencies — Phase 0

## Required build-time dependencies

| Dependency | Version | Purpose | License / distribution note |
|---|---:|---|---|
| CMake | 3.24+ | Configure and generate builds | BSD-3-Clause |
| Visual Studio 2022 / MSVC | 17.x | Windows x64 C++ compiler | Microsoft toolchain terms |
| Windows SDK | 10.0.19041+ recommended | Windows Service Control Manager API | Microsoft SDK terms |
| Qt | 6.5+ | Native UI, QML, IPC, Core and Network | LGPLv3/GPLv3 or commercial; review Qt license and deployment obligations |
| Inno Setup | 6.x | Windows installer | Inno Setup license |

## Qt modules

- Qt6::Core
- Qt6::Gui
- Qt6::Qml
- Qt6::Quick
- Qt6::QuickControls2
- Qt6::Network

## Runtime dependencies

Qt runtime DLLs and QML plugins must be deployed beside the executables using `windeployqt`. The installer script copies the deployed Release tree.

FFmpeg is intentionally not linked or bundled in Phase 0 because media ingest and recording are not yet implemented. It will be introduced in Phase 1 with a pinned build, license review and codec configuration.

OpenCV, ONNX Runtime, CUDA/TensorRT, database drivers, cloud SDKs and map libraries are not Phase 0 dependencies.
