#include "ServiceClient.h"
#include "Protocol.h"

#include <QDesktopServices>
#include <QDir>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include <QStandardPaths>
#include <QUrl>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

ServiceClient::ServiceClient(QObject *parent) : QObject(parent)
{
#ifdef Q_OS_WIN
    const QString base = qEnvironmentVariable("PROGRAMDATA", QStringLiteral("C:/ProgramData"));
#else
    const QString base = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
#endif
    m_dataDirectory = QDir(base).filePath(QStringLiteral("WBSJ WE"));
    m_logDirectory = QDir(m_dataDirectory).filePath(QStringLiteral("logs"));

    connect(&m_socket, &QLocalSocket::connected, this, &ServiceClient::onConnected);
    connect(&m_socket, &QLocalSocket::readyRead, this, &ServiceClient::onReadyRead);
    connect(&m_socket, &QLocalSocket::errorOccurred, this, &ServiceClient::onSocketError);
    connect(&m_pollTimer, &QTimer::timeout, this, &ServiceClient::refresh);
    m_pollTimer.setInterval(3000);
    m_pollTimer.start();
    refresh();
}

void ServiceClient::refresh()
{
    if (m_socket.state() == QLocalSocket::ConnectedState) {
        sendStatusRequest();
        return;
    }
    if (m_socket.state() != QLocalSocket::UnconnectedState)
        return;
    m_state = QStringLiteral("Connecting");
    m_message = QStringLiteral("Looking for WBSJ Recording Service…");
    emit statusChanged();
    m_socket.abort();
    m_socket.connectToServer(QString::fromLatin1(Wbsj::Ipc::ServerName));
}

void ServiceClient::onConnected()
{
    sendStatusRequest();
}

void ServiceClient::sendStatusRequest()
{
    const auto payload = QJsonDocument(Wbsj::Ipc::request(QStringLiteral("status"))).toJson(QJsonDocument::Compact) + '\n';
    m_socket.write(payload);
    m_socket.flush();
}

void ServiceClient::onReadyRead()
{
    m_buffer += m_socket.readAll();
    while (true) {
        const qsizetype end = m_buffer.indexOf('\n');
        if (end < 0)
            break;
        const QByteArray line = m_buffer.left(end).trimmed();
        m_buffer.remove(0, end + 1);
        const auto doc = QJsonDocument::fromJson(line);
        if (!doc.isObject()) {
            setOffline(QStringLiteral("The recording service returned an invalid response."));
            continue;
        }
        const QJsonObject obj = doc.object();
        if (!obj.value(QStringLiteral("ok")).toBool()) {
            setOffline(obj.value(QStringLiteral("error")).toString(QStringLiteral("Service error")));
            continue;
        }
        m_state = obj.value(QStringLiteral("state")).toString(QStringLiteral("Online"));
        m_message = obj.value(QStringLiteral("message")).toString(QStringLiteral("Recording service is responding."));
        m_version = obj.value(QStringLiteral("version")).toString(QStringLiteral("—"));
        m_startedAt = obj.value(QStringLiteral("started_at")).toString(QStringLiteral("—"));
        m_cameras = obj.value(QStringLiteral("configured_cameras")).toInt(0);
        m_recorders = obj.value(QStringLiteral("active_recorders")).toInt(0);
        emit statusChanged();
    }
}

void ServiceClient::onSocketError(QLocalSocket::LocalSocketError)
{
    setOffline(m_socket.errorString());
}

void ServiceClient::setOffline(const QString &message)
{
    m_state = QStringLiteral("Offline");
    m_message = message.isEmpty() ? QStringLiteral("Recording service is unavailable.") : message;
    m_cameras = 0;
    m_recorders = 0;
    emit statusChanged();
}

void ServiceClient::openDataDirectory()
{
    QDir().mkpath(m_dataDirectory);
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_dataDirectory));
}

void ServiceClient::openLogDirectory()
{
    QDir().mkpath(m_logDirectory);
    QDesktopServices::openUrl(QUrl::fromLocalFile(m_logDirectory));
}

void ServiceClient::openWindowsServices()
{
#ifdef Q_OS_WIN
    QProcess::startDetached(QStringLiteral("mmc.exe"), {QStringLiteral("services.msc")});
#else
    QDesktopServices::openUrl(QUrl::fromLocalFile(QStringLiteral("/")));
#endif
}

void ServiceClient::openWebsite()
{
    QDesktopServices::openUrl(QUrl(QStringLiteral("https://wbsj.ir")));
}
