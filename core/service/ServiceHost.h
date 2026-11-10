#pragma once

#include <QObject>
#include <QLocalServer>
#include <QDateTime>
#include <QEventLoop>
#include <QJsonObject>
#include <QFile>
#include <QHash>
#include <QLocalSocket>

class PipeServerThread;

class ServiceHost final : public QObject
{
    Q_OBJECT
public:
    explicit ServiceHost(QObject *parent = nullptr);
    ~ServiceHost() override;
    bool start();
    int runEventLoop();
    int execEventLoop();

public slots:
    void requestStop();

private slots:
    void acceptConnections();
    void readClient();
    void removeClient();

private:
    void writeLog(const QString &message);
    QJsonObject handleRequest(const QJsonObject &request) const;
    QString dataRoot() const;

    QLocalServer m_server;
    QHash<QLocalSocket *, QByteArray> m_buffers;
    QFile m_logFile;
    QDateTime m_startedAt;
    QEventLoop *m_eventLoop{nullptr};
    PipeServerThread *m_pipeThread{nullptr};
};
