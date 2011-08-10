import QtQuick 1.1
import com.meego 1.0
import MeeBible 0.1

PageStackWindow {
    id: window

    initialPage: BibleViewPage {
        id: biblePage

        tools: commonTools

        Loader {
            id: searchDialog

            function load() { source = "SearchDialog.qml" }
            function open() { load(); item.open() }

            Connections {
                target: searchDialog.item
                onPlaceSelected: {
                    searchDialog.item.close()
                    biblePage.loadChapter(bookCode, chapterNo)
                }
            }
        }
    }


    Loader {
        id: languageDialog

        width: 10
        height: 10

        function load() { source = "LanguageDialog.qml" }

        function open() { load(); item.open() }

        Connections {
            target: languageDialog.item
            onAccepted: {
                settings.language = languageDialog.item.language()

                transDialog.open()
            }
        }
    }

    Loader {
        id: transDialog

        width: 10; height: 10

        function load() { source = "TranslationDialog.qml" }

        function open() { load(); item.open() }

        Connections {
            target: transDialog.item
            onAccepted: settings.translation = transDialog.item.translation()
        }
    }


    Loader {
        id: placeDialog

        width: 10; height: 10

        function load() { source = "PlaceDialog.qml" }

        function open() { load(); item.open() }

        Connections {
            target: placeDialog.item
            onAccepted: biblePage.loadChapter(placeDialog.item.bookCode(), placeDialog.item.chapterNo())
        }
    }

    Loader {
        id: fetcherDialog

        width: 10; height: 10

        function load() { source = "FetcherDialog.qml" }
        function open() { load(); item.open() }

        function start() { load(); item.start() }
    }



    ToolBarLayout {
        id: commonTools

        ToolIcon {
            platformIconId: "toolbar-previous"

            onClicked: biblePage.loadPrevChapter()
        }

        ToolIcon {
            platformIconId: "toolbar-next"

            onClicked: biblePage.loadNextChapter()
        }

        ToolIcon {
            platformIconId: "toolbar-update"

            onClicked: placeDialog.open()
        }

        ToolIcon {
            platformIconId: "toolbar-search"

            onClicked: searchDialog.open()
        }

        ToolIcon {
            platformIconId: "toolbar-view-menu"

            onClicked: menu.status == DialogStatus.Closed ? menu.open() : menu.close()
        }
    }


    Menu {
        id: menu

        visualParent: pageStack

        MenuLayout {
            MenuItem {
                text: "Select Language"
                onClicked: languageDialog.open()
            }

            MenuItem {
                text: "Select Translation"
                onClicked: transDialog.open()
            }

            MenuItem {
                text: "Download Bible"
                onClicked: fetcherDialog.start()
            }
        }
    }
}
