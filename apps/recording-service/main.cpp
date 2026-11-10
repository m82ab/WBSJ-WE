#include "../../core/service/ServiceHost.h"

#include <QCoreApplication>
#include <QMetaObject>

#ifdef Q_OS_WIN
#include <windows.h>

namespace {
ServiceHost *g_host = nullptr;
SERVICE_STATUS_HANDLE g_statusHandle = nullptr;
SERVICE_STATUS g_status{};

void reportStatus(DWORD state, DWORD error = NO_ERROR, DWORD waitHint = 0)
{
    g_status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    g_status.dwCurrentState = state;
    g_status.dwWin32ExitCode = error;
    g_status.dwWaitHint = waitHint;
    g_status.dwControlsAccepted = (state == SERVICE_RUNNING) ? SERVICE_ACCEPT_STOP | SERVICE_ACCEPT_SHUTDOWN : 0;
    if (g_statusHandle)
        SetServiceStatus(g_statusHandle, &g_status);
}

void WINAPI controlHandler(DWORD control)
{
    if (control == SERVICE_CONTROL_STOP || control == SERVICE_CONTROL_SHUTDOWN) {
        reportStatus(SERVICE_STOP_PENDING, NO_ERROR, 5000);
        if (g_host)
            QMetaObject::invokeMethod(g_host, "requestStop", Qt::QueuedConnection);
    }
}

void WINAPI serviceMain(DWORD, LPWSTR *)
{
    g_statusHandle = RegisterServiceCtrlHandlerW(L"WBSJWERecordingService", controlHandler);
    if (!g_statusHandle)
        return;
    reportStatus(SERVICE_START_PENDING, NO_ERROR, 5000);
    g_host = new ServiceHost();
    if (!g_host->start()) {
        delete g_host;
        g_host = nullptr;
        reportStatus(SERVICE_STOPPED, ERROR_SERVICE_SPECIFIC_ERROR, 0);
        return;
    }
    reportStatus(SERVICE_RUNNING);
    g_host->execEventLoop();
    delete g_host;
    g_host = nullptr;
    reportStatus(SERVICE_STOPPED);
}
}
#endif

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);
    QCoreApplication::setApplicationName(QStringLiteral("WBSJ WE Recording Service"));
    QCoreApplication::setOrganizationName(QStringLiteral("WBSJ"));
    const bool consoleMode = app.arguments().contains(QStringLiteral("--console"));
    if (consoleMode) {
        ServiceHost host;
        return host.runEventLoop();
    }

#ifdef Q_OS_WIN
    SERVICE_TABLE_ENTRYW table[] = {
        {const_cast<LPWSTR>(L"WBSJWERecordingService"), serviceMain},
        {nullptr, nullptr}
    };
    if (!StartServiceCtrlDispatcherW(table)) {
        const DWORD error = GetLastError();
        if (error == ERROR_FAILED_SERVICE_CONTROLLER_CONNECT) {
            // Running interactively without --console is almost always a packaging mistake.
            return 3;
        }
        return static_cast<int>(error);
    }
    return 0;
#else
    ServiceHost host;
    return host.runEventLoop();
#endif
}
