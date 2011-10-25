TEMPLATE = app

QT += core sql network xml xmlpatterns webkit gui declarative

# DEFINES += DEBUGPATHS

DEFINES += INSTALLPREFIX=\\\"/opt/meebible\\\"


TARGET = meebible
target.path = /opt/meebible/bin
INSTALLS += target



CONFIG += qdeclarative-boostable
CONFIG   += console
CONFIG   -= app_bundle

INCLUDEPATH += /usr/include/applauncherd
LIBS += -licui18n -lsqlite3 -lmdeclarativecache


SOURCES += main.cpp \
    TranslationsList.cpp \
    Language.cpp \
    Languages.cpp \
    Translation.cpp \
    ChapterRequest.cpp \
    Source.cpp \
    NWTSource.cpp \
    NWTranslation.cpp \
    NWTChapterRequest.cpp \
    BibleView.cpp \
    Cache.cpp \
    Paths.cpp \
#    MainWindow.cpp \
    Place.cpp \
#    TranslationDialog.cpp \
    BOSource.cpp \
    BORusTranslation.cpp \
    BOChapterRequest.cpp \
    EasyXml.cpp \
    BibleWebPage.cpp \
    Fetcher.cpp \
#    FetcherDialog.cpp \
    SqliteUnicodeSearch.cpp \
    SearchThread.cpp \
    Settings.cpp

HEADERS += \
    TranslationsList.h \
    Language.h \
    Languages.h \
    Translation.h \
    ChapterRequest.h \
    Source.h \
    NWTSource.h \
    NWTranslation.h \
    NWTChapterRequest.h \
    BibleView.h \
    Cache.h \
    Paths.h \
#    MainWindow.h \
    Place.h \
#    TranslationDialog.h \
    BOSource.h \
    BORusTranslation.h \
    BOChapterRequest.h \
    EasyXml.h \
    BibleWebPage.h \
    Fetcher.h \
#    FetcherDialog.h \
    Utils.h \
    SqliteUnicodeSearch.h \
    SearchThread.h \
    Settings.h

# OTHER_FILES += \
#     createlangs.sql \
#     createnwt.sql \
#     nwt.xslt \
#     style.css \
#     createborus.sql \
#     bo.xslt \
#     script.js \
#     qml/main.qml \
#     qtc_packaging/debian_harmattan/rules \
#     qtc_packaging/debian_harmattan/README \
#     qtc_packaging/debian_harmattan/copyright \
#     qtc_packaging/debian_harmattan/control \
#     qtc_packaging/debian_harmattan/compat \
#     qtc_packaging/debian_harmattan/changelog \
#     db.make \
#     qml/TranslationDialog.qml \
#     qml/SimpleListDelegate.qml \
#     qml/SheetTest.qml \
#     qml/SearchDialog.qml \
#     qml/PlaceDialog.qml \
#     qml/NumberSelector.qml \
#     qml/ModelSelectionDialog.qml \
#     qml/LanguageDialog.qml \
#     qml/FetcherDialog.qml \
#     qml/BibleViewPage.qml

# unix:!symbian:!maemo5 {
#     target.path = /opt/meebible/bin
#     INSTALLS += target
# }
# 
# unix:!symbian:!maemo5 {
#     desktopfile.files = $${TARGET}.desktop
#     desktopfile.path = /usr/share/applications
#     INSTALLS += desktopfile
# }
# 
# unix:!symbian:!maemo5 {
#     icon.files = meebible.png
#     icon.path = /usr/share/icons/hicolor/64x64/apps
#     INSTALLS += icon
# }
# 
# unix:!symbian:!maemo5 {
#     db.files = langs.sqlite nwt.sqlite bo.sqlite
#     db.path = /opt/meebible/share
#     INSTALLS += db
# 
#     qml.files = qml/*
#     qml.path = /opt/meebible/qml
#     INSTALLS += qml
# 
# }
