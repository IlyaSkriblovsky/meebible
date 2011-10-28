TEMPLATE = app

QT += core sql network xml xmlpatterns webkit gui declarative

# DEFINES += DEBUGPATHS

DEFINES += INSTALLPREFIX=\\\"/opt/meebible\\\"


TARGET = meebible
target.path = /opt/meebible/bin
INSTALLS += target



CONFIG += qdeclarative-boostable
CONFIG += console
CONFIG += warn_on
CONFIG -= debug
CONFIG -= app_bundle


INCLUDEPATH += /usr/include/applauncherd
LIBS += -licui18n -lsqlite3 -lmdeclarativecache


SOURCES += main.cpp \
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
    Place.cpp \
    BOSource.cpp \
    BORusTranslation.cpp \
    BOChapterRequest.cpp \
    EasyXml.cpp \
    BibleWebPage.cpp \
    Fetcher.cpp \
    SqliteUnicodeSearch.cpp \
    SearchThread.cpp \
    Settings.cpp

HEADERS += \
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
    Place.h \
    BOSource.h \
    BORusTranslation.h \
    BOChapterRequest.h \
    EasyXml.h \
    BibleWebPage.h \
    Fetcher.h \
    Utils.h \
    SqliteUnicodeSearch.h \
    SearchThread.h \
    Settings.h
