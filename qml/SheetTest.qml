import QtQuick 1.0
import com.meego 1.0
import com.meego.extras 1.0


Sheet {
    id: sheet

    title: [
        SheetButton {
            anchors.left: parent.left
            anchors.verticalCenter: parent.verticalCenter
            text: "Cancel"

            onClicked: sheet.reject()
        },
        SheetButton {
            anchors.right: parent.right
            anchors.verticalCenter: parent.verticalCenter
            text: "Select"
            platformStyle: SheetButtonAccentStyle { }

            onClicked: sheet.accept()
        }
    ]



    content: Rectangle {
        id: content

        anchors.fill: parent

        ListView {
            id: bookList

            clip: true

            model: ListModel {
            }

            focus: true

            delegate: Item {
                id: bookItem

                anchors.left: parent.left
                anchors.right: parent.right
                height: 60

                Label {
                    anchors.left: parent.left
                    anchors.leftMargin: 30
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    text: title

                    font {
                        bold: bookItem.ListView.isCurrentItem
                    }
                }

                MouseArea {
                    anchors.fill: parent

                    onClicked: bookItem.ListView.view.currentIndex = index
                }
            }
        }

        ScrollDecorator { flickableItem: bookList }

        NumberSelector {
            id: chapterSelector

            title: "Chapter:"

            minimum: 1
            maximum: 50
            value: "10"
        }

        NumberSelector {
            id: verseSelector

            title: "Verse:"

            minimum: 1
            maximum: 50
            value: "20"
        }

        state: screen.currentOrientation == Screen.Portrait ? "portrait" : "landscape"

        states: [
            State {
                name: "portrait"

                AnchorChanges {
                    target: chapterSelector

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: verseSelector.top
                }

                AnchorChanges {
                    target: verseSelector

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                }

                AnchorChanges {
                    target: bookList

                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.bottom: chapterSelector.top
                }
            },

            State {
                name: "landscape"

                AnchorChanges {
                    target: chapterSelector

                    anchors.left: parent.horizontalCenter
                    anchors.right: parent.right
                    anchors.bottom: verseSelector.top
                }

                AnchorChanges {
                    target: verseSelector

                    anchors.left: parent.horizontalCenter
                    anchors.right: parent.right
                    anchors.bottom: parent.bottom
                }

                AnchorChanges {
                    target: bookList

                    anchors.left: parent.left
                    anchors.right: parent.horizontalCenter
                    anchors.top: parent.top
                    anchors.bottom: parent.bottom
                }
            }
        ]
    }
}
