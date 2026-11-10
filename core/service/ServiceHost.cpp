#include "ServiceHost.h"
#include "../ipc/Protocol.h"

#include <QCoreApplication>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QThread>
#include <QMutex>
#include <atomic>
#include <functional>
#include <string>
#include <utility>

#ifdef Q_OS_WIN
#include <windows.h>
#include <sddl.h>
#endif

#ifndef WBSJ_WE_VERSION
#define WBSJ_WE_VERSION "0.2.0"
#endif

#ifdef Q_OS_WIN
class PipeServerThread final : public QThread
{
public:
    using Handler = std::function<QJsonObject(const QJsonObject &)>;

    explicit PipeServerThread(Handler handler) : m_handler(std::move(handler)) {}

    void stop()
    {
        m_stop.store(true);
        // Keep a duplicated Win32 thread handle instead of relying on
        // QThread::nativeHandle(), which is not available in every Qt 6.5 SDK.
        m_threadHandleMutex.lock();
        if (m_threadHandle)
            CancelSynchronousIo(m_threadHandle);
        m_threadHandleMutex.unlock();
    }

    bool ready() const { return m_ready.load(); }

protected:
    void run() override
    {
        HANDLE duplicatedThreadHandle = nullptr;
        if (!DuplicateHandle(GetCurrentProcess(), GetCurrentThread(), GetCurrentProcess(),
                             &duplicatedThreadHandle, THREAD_TERMINATE | SYNCHRONIZE,
                             FALSE, 0))
            return;
        m_threadHandleMutex.lock();
        m_threadHandle = duplicatedThreadHandle;
        m_threadHandleMutex.unlock();

        PSECURITY_DESCRIPTOR descriptor = nullptr;
        if (!ConvertStringSecurityDescriptorToSecurityDescriptorW(
                L"D:P(A;;GA;;;SY)(A;;GRGW;;;AU)", SDDL_REVISION_1, &descriptor, nullptr)) {
            m_threadHandleMutex.lock();
            CloseHandle(m_threadHandle);
            m_threadHandle = nullptr;
            m_threadHandleMutex.unlock();
            return;
        }

        SECURITY_ATTRIBUTES attributes{};
        attributes.nLength = sizeof(attributes);
        attributes.lpSecurityDescriptor = descriptor;
        attributes.bInheritHandle = FALSE;
        const std::wstring pipeName = L"\\\\.\\pipe\\WBSJ_WE_RecordingService_v1";

        while (!m_stop.load()) {
            HANDLE pipe = CreateNamedPipeW(
                pipeName.c_str(), PIPE_ACCESS_DUPLEX,
                PIPE_TYPE_BYTE | PIPE_READMODE_BYTE | PIPE_WAIT | PIPE_REJECT_REMOTE_CLIENTS,
                PIPE_UNLIMITED_INSTANCES, 8192, 8192, 0, &attributes);
            if (pipe == INVALID_HANDLE_VALUE)
                break;
            m_ready.store(true);

            const BOOL connected = ConnectNamedPipe(pipe, nullptr)
                ? TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);
            if (connected && !m_stop.load()) {
                QByteArray pending;
                char chunk[1024];
                while (!m_stop.load()) {
                    DWORD received = 0;
                    const BOOL ok = ReadFile(pipe, chunk, sizeof(chunk), &received, nullptr);
                    if (!ok || received == 0)
                        break;
                    pending.append(chunk, static_cast<qsizetype>(received));
                    if (pending.size() > 8192)
                        break;
                    while (true) {
                        const qsizetype end = pending.indexOf('\n');
                        if (end < 0)
                            break;
                        const QByteArray line = pending.left(end).trimmed();
                        pending.remove(0, end + 1);
                        const auto document = QJsonDocument::fromJson(line);
                        const QJsonObject response = document.isObject()
                            ? m_handler(document.object())
                            : Wbsj::Ipc::error(QStringLiteral("Malformed JSON request"));
                        const QByteArray output = QJsonDocument(response).toJson(QJsonDocument::Compact) + '\n';
                        DWORD written = 0;
                        if (!WriteFile(pipe, output.constData(), static_cast<DWORD>(output.size()), &written, nullptr))
                            break;
                    }
                }
            }
            if (!m_stop.load())
                FlushFileBuffers(pipe);
            DisconnectNamedPipe(pipe);
            CloseHandle(pipe);
        }
        LocalFree(descriptor);
        m_threadHandleMutex.lock();
        if (m_threadHandle) {
            CloseHandle(m_threadHandle);
            m_threadHandle = nullptr;
        }
        m_threadHandleMutex.unlock();
    }

private:
    Handler m_handler;
    std::atomic_bool m_stop{false};
    std::atomic_bool m_ready{false};
    QMutex m_threadHandleMutex;
    HANDLE m_threadHandle{nullptr};
};
#endif

ServiceHost::ServiceHost(QObject *parent) : QObject(parent)
{
#ifndef Q_OS_WIN
    connect(&m_server, &QLocalServer::newConnection, this, &ServiceHost::acceptConnections);
#endif
}

ServiceHost::~ServiceHost()
{
#ifdef Q_OS_WIN
    if (m_pipeThread) {
        m_pipeThread->stop();
        m_pipeThread->wait();
        delete m_pipeThread;
        m_pipeThread = nullptr;
    }
#endif
}

QString ServiceHost::dataRoot() const
{
#ifdef Q_OS_WIN
    return QDir(qEnvironmentVariable("PROGRAMDATA", QStringLiteral("C:/ProgramData"))).filePath(QStringLiteral("WBSJ WE"));
#else
    return QDir(QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)).filePath(QStringLiteral("WBSJ WE"));
#endif
}

bool ServiceHost::start()
{
    const QString root = dataRoot();
    const QString logs = QDir(root).filePath(QStringLiteral("logs"));
    if (!QDir().mkpath(logs))
        return false;
    m_logFile.setFileName(QDir(logs).filePath(QStringLiteral("recording-service.log")));
    if (!m_logFile.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text))
        return false;

    m_startedAt = QDateTime::currentDateTimeUtc();
#ifdef Q_OS_WIN
    // Windows service runs as LocalSystem. A dedicated named pipe ACL grants
    // status-only access to authenticated local users and rejects remote clients.
    m_pipeThread = new PipeServerThread([this](const QJsonObject &request) {
        return handleRequest(request);
    });
    m_pipeThread->start();
    for (int attempt = 0; attempt < 100 && !m_pipeThread->ready() && m_pipeThread->isRunning(); ++attempt)
        QThread::msleep(10);
    if (!m_pipeThread->ready()) {
        writeLog(QStringLiteral("Named pipe initialization failed."));
        m_pipeThread->stop();
        m_pipeThread->wait();
        delete m_pipeThread;
        m_pipeThread = nullptr;
        return false;
    }
#else
    QLocalServer::removeServer(QString::fromLatin1(Wbsj::Ipc::ServerName));
    m_server.setSocketOptions(QLocalServer::UserAccessOption);
    if (!m_server.listen(QString::fromLatin1(Wbsj::Ipc::ServerName))) {
        writeLog(QStringLiteral("IPC listen failed: ") + m_server.errorString());
        return false;
    }
#endif
    writeLog(QStringLiteral("Recording service started. Version ") + QStringLiteral(WBSJ_WE_VERSION));
    return true;
}

int ServiceHost::runEventLoop()
{
    if (!start())
        return 2;
    return execEventLoop();
}

int ServiceHost::execEventLoop()
{
    QEventLoop loop;
    m_eventLoop = &loop;
    const int result = loop.exec();
    m_eventLoop = nullptr;
    return result;
}

void ServiceHost::requestStop()
{
    writeLog(QStringLiteral("Recording service stopping."));
    for (auto *socket : m_buffers.keys()) {
        socket->disconnectFromServer();
        socket->deleteLater();
    }
    m_buffers.clear();
#ifdef Q_OS_WIN
    if (m_pipeThread) {
        m_pipeThread->stop();
        m_pipeThread->wait();
        delete m_pipeThread;
        m_pipeThread = nullptr;
    }
#else
    m_server.close();
    QLocalServer::removeServer(QString::fromLatin1(Wbsj::Ipc::ServerName));
#endif
    if (m_eventLoop)
        m_eventLoop->quit();
}

void ServiceHost::acceptConnections()
{
    while (m_server.hasPendingConnections()) {
        auto *socket = m_server.nextPendingConnection();
        if (!socket)
            continue;
        m_buffers.insert(socket, QByteArray{});
        connect(socket, &QLocalSocket::readyRead, this, &ServiceHost::readClient);
        connect(socket, &QLocalSocket::disconnected, this, &ServiceHost::removeClient);
    }
}

void ServiceHost::readClient()
{
    auto *socket = qobject_cast<QLocalSocket *>(sender());
    if (!socket || !m_buffers.contains(socket))
        return;
    auto &buffer = m_buffers[socket];
    buffer += socket->readAll();
    if (buffer.size() > 8192) {
        socket->write(QJsonDocument(Wbsj::Ipc::error(QStringLiteral("Request too large"))).toJson(QJsonDocument::Compact) + '\n');
        socket->disconnectFromServer();
        return;
    }
    while (true) {
        const qsizetype end = buffer.indexOf('\n');
        if (end < 0)
            break;
        const QByteArray line = buffer.left(end).trimmed();
        buffer.remove(0, end + 1);
        const auto doc = QJsonDocument::fromJson(line);
        QJsonObject response;
        if (!doc.isObject()) {
            response = Wbsj::Ipc::error(QStringLiteral("Malformed JSON request"));
        } else {
            response = handleRequest(doc.object());
        }
        socket->write(QJsonDocument(response).toJson(QJsonDocument::Compact) + '\n');
        socket->flush();
    }
}

QJsonObject ServiceHost::handleRequest(const QJsonObject &request) const
{
    if (request.value(QStringLiteral("protocol")).toInt(-1) != Wbsj::Ipc::ProtocolVersion)
        return Wbsj::Ipc::error(QStringLiteral("Unsupported IPC protocol version"));
    const QString command = request.value(QStringLiteral("command")).toString();
    if (command == QStringLiteral("ping"))
        return {{QStringLiteral("ok"), true}, {QStringLiteral("pong"), true}};
    if (command != QStringLiteral("status"))
        return Wbsj::Ipc::error(QStringLiteral("Command is not available in Phase 0"));

    return {{QStringLiteral("ok"), true},
            {QStringLiteral("state"), QStringLiteral("Online")},
            {QStringLiteral("message"), QStringLiteral("Service is running. Camera and recording engines are not enabled in Phase 0.")},
            {QStringLiteral("version"), QStringLiteral(WBSJ_WE_VERSION)},
            {QStringLiteral("started_at"), m_startedAt.toString(Qt::ISODate)},
            {QStringLiteral("configured_cameras"), 0},
            {QStringLiteral("active_recorders"), 0},
            {QStringLiteral("phase"), 0}};
}

void ServiceHost::removeClient()
{
    auto *socket = qobject_cast<QLocalSocket *>(sender());
    if (!socket)
        return;
    m_buffers.remove(socket);
    socket->deleteLater();
}

void ServiceHost::writeLog(const QString &message)
{
    if (!m_logFile.isOpen())
        return;
    const QByteArray line = QDateTime::currentDateTimeUtc().toString(Qt::ISODate).toUtf8() + " " + message.toUtf8() + '\n';
    m_logFile.write(line);
    m_logFile.flush();
}
