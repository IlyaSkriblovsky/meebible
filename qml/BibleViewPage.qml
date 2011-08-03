import QtQuick 1.1
import com.meego 1.0
import MeeBible 0.1

Page {
    id: page


    function setTranslation(translation) { bibleView.setTranslation(translation) }
    function loadChapter(bookCode, chapterNo) { bibleView.loadChapter(bookCode, chapterNo) }
    function loadPrevChapter() { bibleView.loadPrevChapter() }
    function loadNextChapter() { bibleView.loadNextChapter() }


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
                onChapterLoadingError: { flickable.contentY = 0; page.state = "normal" }
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
