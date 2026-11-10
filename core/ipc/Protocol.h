#pragma once

#include <QJsonObject>
#include <QString>

namespace Wbsj::Ipc {
inline constexpr auto ServerName = "WBSJ_WE_RecordingService_v1";
inline constexpr int ProtocolVersion = 1;

inline QJsonObject request(const QString &command)
{
    return {{QStringLiteral("protocol"), ProtocolVersion},
            {QStringLiteral("command"), command}};
}

inline QJsonObject error(const QString &message)
{
    return {{QStringLiteral("ok"), false}, {QStringLiteral("error"), message}};
}
}
