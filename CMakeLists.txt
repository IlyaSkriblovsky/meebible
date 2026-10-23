cmake_minimum_required(VERSION 3.30)
project(meebible)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Network Xml Gui Quick Core5compat WebEngineWidgets)
find_package(SQLite3 REQUIRED)

add_definitions(-DDESKTOP)


add_executable(meebible
    src/main.cpp
    src/Language.cpp
    src/Languages.cpp
    src/Translation.cpp
    src/ChapterRequest.cpp
    src/BibleViewController.cpp
    src/Cache.cpp
    src/Paths.cpp
    src/Place.cpp
    src/Settings.cpp
    src/MetaInfoParser.cpp
    src/TranslationInfoParser.cpp
    src/MetaInfoLoader.cpp
    src/Bookmarks.cpp
    src/PlaceAccesser.cpp
    src/MappedFile.cpp
    src/Indexer.cpp
    src/SearchResult.cpp
    src/SearchResultAccesser.cpp
    src/Highlighter.cpp
    src/SearchQueryParser.cpp
    src/SearchThread.cpp
    src/IndexRebuildThread.cpp
    src/MessageHelper.cpp
)

qt_add_resources(meebible "qml"
    PREFIX "/"
    BASE qml-desktop
    FILES
        qml-desktop/main.qml
        qml-desktop/BibleViewPage.qml
        qml-desktop/AboutPage.qml
        qml-desktop/Header.qml
        qml-desktop/header.png
        qml-desktop/script.js
        qml-desktop/style.css
        qml-desktop/PlaceDialog.qml
        qml-desktop/SimpleListDelegate.qml
        qml-desktop/GridPlaceDialog.qml
        qml-desktop/BookColors.js
        qml-desktop/NumberGridSelector.qml
        qml-desktop/BookGridDelegate.qml
        qml-desktop/verse-actions-close-symbian.png
        qml-desktop/BookmarksPage.qml
        qml-desktop/SubtitledListDelegate.qml
        qml-desktop/SettingsPage.qml
        qml-desktop/QueryDialog.qml
        qml-desktop/LabeledSwitch.qml
        qml-desktop/TumblerItem.qml
        qml-desktop/ModelSelectionDialog.qml
        qml-desktop/ButtonWithHelp.qml
        qml-desktop/HelpButton.qml
        qml-desktop/UniDialog.qml
        qml-desktop/tumbler-indicator.png
        qml-desktop/tumbler-indicator-inverted.png
        qml-desktop/help-button.png
        qml-desktop/help-button-inverted.png
        qml-desktop/icon-m-common-dialog-close.png
        qml-desktop/LanguageDialog.qml
)

target_link_libraries(meebible
    Qt6::Core
    Qt6::Network
    Qt6::Xml
    Qt6::Gui
    Qt6::Quick
    Qt6::Core5Compat
    Qt6::WebEngineWidgets
    ${SQLite3_LIBRARIES}
)