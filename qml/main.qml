import QtQuick 1.1
import com.meego 1.0
import MeeBible 0.1

PageStackWindow {
    id: window

    initialPage: BibleViewPage {
        id: biblePage

        tools: commonTools

        SearchDialog {
            id: searchDialog

            onPlaceSelected: {
                close()
                biblePage.loadChapter(bookCode, chapterNo)
            }
        }
    }


    LanguageDialog {
        id: languageDialog

        onAccepted: {
            transDialog.model = language()
            transDialog.open()
        }
    }

    TranslationDialog {
        id: transDialog

        onAccepted: {
            biblePage.setTranslation(translation())

            placeDialog.bookModel = translation()
        }
    }


    PlaceDialog {
        id: placeDialog

        onAccepted: {
            console.log('(' + bookCode() + ') ' + bookName() + ' ' + chapterNo() + ':' + verseNo())
            biblePage.loadChapter(bookCode(), chapterNo())
        }
    }

    FetcherDialog {
        id: fetcherDialog
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

//            onClicked: cache.search(transDialog.translation(), "gelukk")
            onClicked: {
                searchDialog.translation = transDialog.translation()
                searchDialog.open()
            }
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
                onClicked: fetcherDialog.start(transDialog.translation())
            }
        }
    }
}
