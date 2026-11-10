from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]

def read(path):
    return (ROOT / path).read_text(encoding="utf-8")

cmake = read("CMakeLists.txt")
main = read("apps/desktop/main.cpp")
qml = read("apps/desktop/qml/Main.qml")
service = read("core/service/ServiceHost.cpp")
workflow = read(".github/workflows/windows-build.yml")
client = read("core/ipc/ServiceClient.cpp")

assert "find_package(Qt6 6.5 REQUIRED" in cmake
assert "qt_add_qml_module" in cmake
assert "WBSJ_WE_RecordingService" in cmake
assert "loadFromModule" in main
assert "QDesktopServices" not in main
assert "http://" not in qml and "https://" not in qml
assert "QLocalServer" in service and "QLocalSocket" in client
assert "CreateNamedPipeW" in service and "PIPE_REJECT_REMOTE_CLIENTS" in service
assert "DuplicateHandle" in service and "CancelSynchronousIo" in service
assert "reinterpret_cast<HANDLE>(nativeHandle())" not in service
assert "windows-2022" in workflow and "win64_msvc2022_64" in workflow
assert "WBSJWE.iss" in workflow
assert "ConvertStringSecurityDescriptorToSecurityDescriptorW" in service
assert 'command == QStringLiteral("ping")' in service
assert 'command != QStringLiteral("status")' in service
assert 'configured_cameras"), 0' in service
assert 'active_recorders"), 0' in service
assert "Phase 1" in qml or "Phase 0" in qml
assert "Recording engine" in qml
print("Phase 0 static checks: PASS")
