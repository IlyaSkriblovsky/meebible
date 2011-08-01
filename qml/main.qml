import QtQuick 1.1
import MeeBible 0.1
import com.meego 1.0

PageStackWindow {

    initialPage: Page {
        id: page

        Rectangle {
            anchors.fill: parent

            clip: true

            Flickable {
                id: flickable

                anchors.fill: parent

                contentWidth: bibleView.width
                contentHeight:  bibleView.height

                BibleView {
                    id: bibleView

                    width: page.width

                    resizesToContents: true
                    preferredWidth: page.width

                    url: "about:blank"

                    onChapterLoaded: { flickable.contentY = 0; page.state = "normal" }
                    onLoading: page.state = "loading"

                    Component.onCompleted: loadChapter('mt', 5)
                }
            }
            ScrollDecorator { flickableItem: flickable }
        }


        Rectangle {
            id: busyIndicator

            anchors.fill: parent

            color: '#fff'

            BusyIndicator {
                anchors.centerIn: parent

                running: true

                platformStyle: BusyIndicatorStyle {
                    size: "large"
                }
            }

            state: "invisible"

            states: [
                State {
                    name: "invisible"

                    PropertyChanges {
                        target: busyIndicator
                        opacity: 0.0
                    }
                },
                State {
                    name: "visible"

                    PropertyChanges {
                        target: busyIndicator
                        opacity: 0.7
                    }
                }
            ]

            transitions: Transition {
                NumberAnimation {
                    properties: "opacity"
                    duration: 100
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
                bibleView.setTranslation(translation())

                placeDialog.model = translation()
            }
        }


        PlaceDialog {
            id: placeDialog

            onAccepted: {
                console.log(bookCode())
            }
        }

        Loader {
            id: sheetTest
        }


        tools: ToolBarLayout {
            ToolIcon {
                platformIconId: "toolbar-previous"

                onClicked: bibleView.loadPrevChapter()
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
                platformIconId: "toolbar-next"

                onClicked: bibleView.loadNextChapter()
            }
        }


        state: "normal"

        states: [
            State {
                name: "normal"

                PropertyChanges {
                    target: busyIndicator
                    state: "invisible"
                }
            },
            State {
                name: "loading"

                PropertyChanges {
                    target: busyIndicator
                    state: "visible"
                }
            }
        ]
    }
}
