#pragma once

#include <QObject>
#include <QLocalSocket>
#include <QTimer>
#include <QVariantMap>

class ServiceClient final : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString serviceState READ serviceState NOTIFY statusChanged)
    Q_PROPERTY(QString serviceMessage READ serviceMessage NOTIFY statusChanged)
    Q_PROPERTY(QString serviceVersion READ serviceVersion NOTIFY statusChanged)
    Q_PROPERTY(QString startedAt READ startedAt NOTIFY statusChanged)
    Q_PROPERTY(int configuredCameras READ configuredCameras NOTIFY statusChanged)
    Q_PROPERTY(int activeRecorders READ activeRecorders NOTIFY statusChanged)
    Q_PROPERTY(QString dataDirectory READ dataDirectory CONSTANT)
    Q_PROPERTY(QString logDirectory READ logDirectory CONSTANT)

public:
    explicit ServiceClient(QObject *parent = nullptr);

    QString serviceState() const { return m_state; }
    QString serviceMessage() const { return m_message; }
    QString serviceVersion() const { return m_version; }
    QString startedAt() const { return m_startedAt; }
    int configuredCameras() const { return m_cameras; }
    int activeRecorders() const { return m_recorders; }
    QString dataDirectory() const { return m_dataDirectory; }
    QString logDirectory() const { return m_logDirectory; }

    Q_INVOKABLE void refresh();
    Q_INVOKABLE void openDataDirectory();
    Q_INVOKABLE void openLogDirectory();
    Q_INVOKABLE void openWindowsServices();
    Q_INVOKABLE void openWebsite();

signals:
    void statusChanged();
    void connectionError(const QString &message);

private slots:
    void onConnected();
    void onReadyRead();
    void onSocketError(QLocalSocket::LocalSocketError error);

private:
    void setOffline(const QString &message);
    void sendStatusRequest();

    QLocalSocket m_socket;
    QTimer m_pollTimer;
    QByteArray m_buffer;
    QString m_state{QStringLiteral("Checking")};
    QString m_message{QStringLiteral("Connecting to the recording service…")};
    QString m_version{QStringLiteral("—")};
    QString m_startedAt{QStringLiteral("—")};
    QString m_dataDirectory;
    QString m_logDirectory;
    int m_cameras{0};
    int m_recorders{0};
};
