[CmdletBinding()]
param(
    [string]$Qt6_DIR = $env:Qt6_DIR,
    [string]$BuildDir = 'build-windows',
    [ValidateSet('Debug','Release')][string]$Configuration = 'Release',
    [string]$Generator = 'Visual Studio 17 2022'
)
$ErrorActionPreference = 'Stop'
if (-not $Qt6_DIR) { throw 'Set Qt6_DIR to the Qt 6.5+ MSVC installation, e.g. C:\Qt\6.8.3\msvc2022_64\lib\cmake\Qt6' }
$qtCmake = Join-Path $Qt6_DIR 'Qt6Config.cmake'
if (-not (Test-Path $qtCmake)) { throw "Qt6Config.cmake not found at $qtCmake" }
cmake -S . -B $BuildDir -G $Generator -A x64 "-DQt6_DIR=$Qt6_DIR"
if ($LASTEXITCODE -ne 0) { throw 'CMake configure failed.' }
cmake --build $BuildDir --config $Configuration --parallel
if ($LASTEXITCODE -ne 0) { throw 'Build failed.' }
$desktop = Join-Path $BuildDir "$Configuration\WBSJ_WE.exe"
$service = Join-Path $BuildDir "$Configuration\WBSJ_WE_RecordingService.exe"
if (-not (Test-Path $desktop) -or -not (Test-Path $service)) { throw 'Expected binaries were not produced.' }
$windeployqt = Join-Path (Split-Path (Split-Path (Split-Path $Qt6_DIR -Parent) -Parent) -Parent) 'bin\windeployqt.exe'
if (-not (Test-Path $windeployqt)) { $windeployqt = (Get-Command windeployqt.exe -ErrorAction SilentlyContinue).Source }
if (-not $windeployqt) { throw 'windeployqt.exe not found. Add the Qt bin directory to PATH.' }
& $windeployqt --qmldir apps/desktop/qml --release $desktop
if ($LASTEXITCODE -ne 0) { throw 'windeployqt failed.' }
Write-Host "Desktop: $desktop"
Write-Host "Service: $service"
Write-Host 'Qt deployment completed. Build the installer with packaging/windows/WBSJWE.iss.'
