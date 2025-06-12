#include <QApplication>
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>

#include <QQuickImageProvider>
#include <QStyle>

#include "Paths.h"
#include "Cache.h"
#include "Languages.h"
#include "Settings.h"
#include "MetaInfoLoader.h"
#include "BibleViewController.h"
#include "Place.h"
#include "PlaceAccesser.h"
#include "Bookmarks.h"


int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);

    app.setOrganizationName("MeeBible");
    app.setOrganizationDomain("meebible.org");
    app.setApplicationName("MeeBible");

    Paths::init();

    Cache cache;
    MetaInfoLoader metaInfoLoader;
    Languages languages;
    Settings settings(&languages);
    Bookmarks bookmarks;

    qmlRegisterType<BibleViewController>("MeeBible", 0, 1, "BibleViewController");
    qRegisterMetaType<Place>();

    QQmlApplicationEngine engine;
    engine.rootContext()->setContextProperty("freeversion", false);
    engine.rootContext()->setContextProperty("SYMBIAN", true);
    engine.rootContext()->setContextProperty("settings", &settings);
    engine.rootContext()->setContextProperty("languages", &languages);
    engine.rootContext()->setContextProperty("bookmarks", &bookmarks);

    PlaceAccesser placeAccesser;
    engine.rootContext()->setContextProperty("placeAccesser", &placeAccesser);

    const QUrl url(QStringLiteral("qrc:/main.qml"));
    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreated,
        &app,
        [url](QObject *obj, const QUrl &objUrl) {
            if (!obj && url == objUrl)
                QCoreApplication::exit(-1);
        },
        Qt::QueuedConnection);
    engine.load(url);

    return QApplication::exec();
}
