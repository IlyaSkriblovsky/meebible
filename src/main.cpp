#include <QApplication>
#include <QDebug>
#include <QtDeclarative>
#include <QtGlobal>

#ifndef SYMBIAN
    #include <MDeclarativeCache>
#endif

#include <QTranslator>
#include <QLocale>

#include "Paths.h"
#include "Cache.h"
#include "Languages.h"
#include "Language.h"
#include "Translation.h"
#include "BibleView.h"
#include "Fetcher2.h"
#include "Settings.h"
#include "MetaInfoLoader.h"
#include "Feedback.h"
#include "Bookmarks.h"
#include "Place.h"
#include "PlaceAccesser.h"
#include "StartupTracker.h"
#include "SearchResultAccesser.h"

#ifdef IAPDONATION
    #include "IAPDonation.h"
#endif


#ifdef SYMBIAN
    #include <cstdio>
    void symbianMessageOutput(QtMsgType type, const char *msg)
    {
        FILE *f = fopen("c:\\meebible.log", "a");
        switch (type)
        {
            case QtDebugMsg:    fprintf(f, "debug> %s\n", msg); break;
            case QtWarningMsg:  fprintf(f, "warn > %s\n", msg); break;
            case QtCriticalMsg: fprintf(f, "crit > %s\n", msg); break;
            case QtFatalMsg:    fprintf(f, "fatal> %s\n", msg); abort(); break;
        }
        fclose(f);
    }
#endif




class CacheConvertThread: public QThread
{
    public:
        CacheConvertThread(Cache* cache): _cache(cache) { }

    protected:
        virtual void run()
        {
            _cache->convertOldCacheDB();
        }

    private:
        Cache* _cache;
};




Q_DECL_EXPORT int main(int argc, char *argv[])
{
    #ifdef SYMBIAN
        #ifdef DEBUG
            qInstallMsgHandler(symbianMessageOutput);
        #endif
    #endif

    #ifndef SYMBIAN
        QApplication *app = MDeclarativeCache::qApplication(argc, argv);
    #else
        QApplication *app = new QApplication(argc, argv);
    #endif
    app->setOrganizationName("MeeBible");
    app->setApplicationName("MeeBible");

    Paths::init();

    QTranslator translator;
    translator.load(Paths::translationFile(QLocale::system().name()));
    app->installTranslator(&translator);

    Cache cache;
    MetaInfoLoader metaInfoLoader;

    Languages languages;

    Settings settings(&languages);

    Feedback feedback;

    Bookmarks bookmarks;

    PlaceAccesser placeAccesser;
    SearchResultAccesser searchResultAccesser;

    QDeclarativeEngine engine;


    qmlRegisterType<Language>();
    qmlRegisterUncreatableType<Translation>("MeeBible", 0, 1, "Translation", "Translation is abstract");
    qmlRegisterType<BibleView>("MeeBible", 0, 1, "BibleView");
    qmlRegisterType<Fetcher2>("MeeBible", 0, 1, "Fetcher");

    qRegisterMetaType<Place>();


    #ifndef SYMBIAN
        QDeclarativeView* view = MDeclarativeCache::qDeclarativeView();
    #else
        QDeclarativeView* view = new QDeclarativeView();
    #endif
    view->setAttribute(Qt::WA_NoSystemBackground);


    view->rootContext()->setContextProperty("languages", &languages);
    view->rootContext()->setContextProperty("cache", &cache);
    view->rootContext()->setContextProperty("metaInfoLoader", &metaInfoLoader);
    view->rootContext()->setContextProperty("settings", &settings);
    view->rootContext()->setContextProperty("feedback", &feedback);
    view->rootContext()->setContextProperty("bookmarks", &bookmarks);
    view->rootContext()->setContextProperty("placeAccesser", &placeAccesser);
    view->rootContext()->setContextProperty("searchResultAccesser", &searchResultAccesser);

    #ifdef FREEVERSION
        view->rootContext()->setContextProperty("freeversion", QVariant(true));
    #else
        view->rootContext()->setContextProperty("freeversion", QVariant(false));
    #endif


    #ifdef SYMBIAN
        qDebug() << "Symbian";
        view->rootContext()->setContextProperty("SYMBIAN", QVariant(true));
    #else
        view->rootContext()->setContextProperty("SYMBIAN", QVariant(false));
    #endif

    #ifdef NOSHARE
        qDebug() << "Verse sharing disabled";
        view->rootContext()->setContextProperty("NOSHARE", QVariant(true));
    #else
        view->rootContext()->setContextProperty("NOSHARE", QVariant(false));
    #endif


    #ifdef IAPDONATION
        // IAPDonation iapDonation;
        qDebug() << "With IAP Donation";
        qmlRegisterType<IAPDonation>("MeeBible", 0, 1, "IAPDonation");
        view->rootContext()->setContextProperty("IAPDONATION", QVariant(true));
    #else
        view->rootContext()->setContextProperty("IAPDONATION", QVariant(false));
    #endif



    QDeclarativeView* convertDialog = 0;
    if (cache.oldStorageFound())
    {
        convertDialog = new QDeclarativeView();
        convertDialog->setAttribute(Qt::WA_NoSystemBackground);
        convertDialog->setSource(QUrl::fromLocalFile(Paths::qmlConvertDialog()));
        convertDialog->showFullScreen();

        QEventLoop* eventLoop = new QEventLoop;
        CacheConvertThread* thread = new CacheConvertThread(&cache);
        QObject::connect(thread, SIGNAL(finished()), eventLoop, SLOT(quit()));
        thread->start();
        eventLoop->exec();
        thread->wait();
        delete thread;
        delete eventLoop;
    }

    view->setSource(QUrl::fromLocalFile(Paths::qmlMain()));

    view->showFullScreen();

    delete convertDialog;



    StartupTracker startupTracker;
    startupTracker.sendStartupInfo();



    return app->exec();
}
