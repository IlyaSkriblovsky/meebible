import QtQuick 1.1
import com.meego 1.0
import MeeBible 0.1

PageStackWindow {
    id: window

    initialPage: BibleViewPage {
        id: biblePage

        tools: commonTools
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
            platformIconId: "toolbar-select-text"

            onClicked: languageDialog.open()
        }

        ToolIcon {
            platformIconId: "toolbar-view-menu"

            onClicked: transDialog.open()
        }

        ToolIcon {
            id: updatebutton
            platformIconId: "toolbar-update"

            onClicked: {
                placeDialog.open()
            }
        }

        ToolIcon {
            id: fetcherbutton
            platformIconId: "toolbar-down"

            onClicked: {
                fetcherDialog.start(transDialog.translation())
            }
        }

        ToolIcon {
            platformIconId: "toolbar-next"

            onClicked: biblePage.loadNextChapter()
        }
    }
}
