# QT += core sql network xml xmlpatterns webkit gui declarative
QT += core sql network xml webkit gui declarative

# DEFINES += DEBUG

nosearch:DEFINES += NOSEARCH
noshare: DEFINES += NOSHARE
iapdonation: DEFINES += IAPDONATION

free:DEFINES += FREEVERSION

symbian: DEFINES += SYMBIAN


!symbian: DEFINES += INSTALLPREFIX=\\\"$$INSTALLDIR\\\"

iapdonation: CONFIG += inapppurchase


TARGET = meebible
target.path = $$INSTALLDIR/bin
INSTALLS += target



CONFIG += console
CONFIG += warn_on
CONFIG -= debug
CONFIG -= app_bundle


LIBS += -lsqlite3
!nosearch: LIBS += -licui18n

!symbian {
    CONFIG += shareuiinterface-maemo-meegotouch mdatauri

    CONFIG += qdeclarative-boostable
    INCLUDEPATH += /usr/include/applauncherd
    LIBS += -lmdeclarativecache
}


HEADERS +=                      \
    src/Language.h              \
    src/Languages.h             \
    src/Translation.h           \
    src/ChapterRequest.h        \
    src/BibleView.h             \
    src/Cache.h                 \
    src/Paths.h                 \
    src/Place.h                 \
    src/BibleWebPage.h          \
    src/Fetcher.h               \
    src/Utils.h                 \
    src/Settings.h              \
    src/MetaInfoParser.h        \
    src/TranslationInfoParser.h \
    src/MetaInfoLoader.h        \
    src/Feedback.h              \
    src/NetworkUtils.h          \
    src/StartupTracker.h        \
    src/Bookmarks.h             \
    src/PlaceAccesser.h

!nosearch:HEADERS +=            \
    src/SqliteUnicodeSearch.h   \
    src/SearchThread.h

iapdonation:HEADERS +=          \
    src/IAPDonation.h

SOURCES +=                      \
    src/main.cpp                \
    src/Language.cpp            \
    src/Languages.cpp           \
    src/Translation.cpp         \
    src/ChapterRequest.cpp      \
    src/BibleView.cpp           \
    src/Cache.cpp               \
    src/Paths.cpp               \
    src/Place.cpp               \
    src/BibleWebPage.cpp        \
    src/Fetcher.cpp             \
    src/Settings.cpp            \
    src/MetaInfoParser.cpp      \
    src/TranslationInfoParser.cpp\
    src/MetaInfoLoader.cpp      \
    src/Feedback.cpp            \
    src/NetworkUtils.cpp        \
    src/StartupTracker.cpp      \
    src/Bookmarks.cpp           \
    src/PlaceAccesser.cpp

!nosearch:SOURCES +=            \
    src/SqliteUnicodeSearch.cpp \
    src/SearchThread.cpp

iapdonation:SOURCES +=          \
    src/IAPDonation.cpp
