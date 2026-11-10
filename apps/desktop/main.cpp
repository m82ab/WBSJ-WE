#include "../../core/ipc/ServiceClient.h"

#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QIcon>
#include <QUrl>

#ifndef WBSJ_WE_VERSION
#define WBSJ_WE_VERSION "0.2.0"
#endif

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    QGuiApplication::setApplicationName(QStringLiteral("WBSJ WE"));
    QGuiApplication::setOrganizationName(QStringLiteral("WBSJ"));
    QGuiApplication::setApplicationVersion(QStringLiteral(WBSJ_WE_VERSION));
    QGuiApplication::setWindowIcon(QIcon(QStringLiteral(":/WBSJ_WE_icon.png")));

    ServiceClient backend;
    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty(QStringLiteral("backend"), &backend);
    engine.rootContext()->setContextProperty(QStringLiteral("appVersion"), QStringLiteral(WBSJ_WE_VERSION));
    engine.loadFromModule(QStringLiteral("WBSJWE"), QStringLiteral("Main"));
    if (engine.rootObjects().isEmpty())
        return 1;
    return app.exec();
}
