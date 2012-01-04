TEMPLATE = app

QT += core sql network xml xmlpatterns webkit gui declarative

# DEFINES += DEBUGPATHS

free:DEFINES += FREEVERSION INSTALLPREFIX=\\\"/opt/meebible-free\\\"
!free:DEFINES += INSTALLPREFIX=\\\"/opt/meebible\\\"


TARGET = meebible
free:target.path = /opt/meebible-free/bin
!free:target.path = /opt/meebible/bin
INSTALLS += target



CONFIG += qdeclarative-boostable
CONFIG += console
CONFIG += warn_on
CONFIG -= debug
CONFIG -= app_bundle


INCLUDEPATH += /usr/include/applauncherd
LIBS += -licui18n -lsqlite3 -lmdeclarativecache


HEADERS +=                  \
    Language.h              \
    Languages.h             \
    Translation.h           \
    ChapterRequest.h        \
    Source.h                \
    NWTSource.h             \
    NWTranslation.h         \
    NWTChapterRequest.h     \
    BibleView.h             \
    Cache.h                 \
    Paths.h                 \
    Place.h                 \
    EasyXml.h               \
    BibleWebPage.h          \
    Fetcher.h               \
    Utils.h                 \
    SqliteUnicodeSearch.h   \
    SearchThread.h          \
    Settings.h              \
    SimpleSource.h          \
    SimpleTranslation.h     \
    BLVSource2.h            \
    BOSource2.h             \
    CCArabicSource2.h       \
    KJBOSource.h

SOURCES += main.cpp         \
    Language.cpp            \
    Languages.cpp           \
    Translation.cpp         \
    ChapterRequest.cpp      \
    Source.cpp              \
    NWTSource.cpp           \
    NWTranslation.cpp       \
    NWTChapterRequest.cpp   \
    BibleView.cpp           \
    Cache.cpp               \
    Paths.cpp               \
    Place.cpp               \
    EasyXml.cpp             \
    BibleWebPage.cpp        \
    Fetcher.cpp             \
    SqliteUnicodeSearch.cpp \
    SearchThread.cpp        \
    Settings.cpp            \
    SimpleSource.cpp        \
    SimpleTranslation.cpp   \
    BLVSource2.cpp          \
    BOSource2.cpp           \
    CCArabicSource2.cpp     \
    KJBOSource.cpp
