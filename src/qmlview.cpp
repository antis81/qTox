#include "qmlview.h"

#include <QGuiApplication>
#include <QQmlContext>
#include <QQmlEngine>
#include <QQuickItem>
#include <QQuickView>

#include "core/core.h"

QmlView::QmlView()
{
    Core* core = Core::getInstance();

    view.rootContext()->setContextProperty(QStringLiteral("Core"), core);
    view.setSource(QUrl(QStringLiteral("qrc:/qml/DEMO.qml")));
    view.setResizeMode(QQuickView::SizeRootObjectToView);
    view.setTitle(qApp->applicationName());
}
