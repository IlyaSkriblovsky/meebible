import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtQuick.Window 2.2
import MeeBible 0.1

import "BookColors.js" as BookColors


Dialog {
    id: dialog

    title: qsTr("Select book and verse")
    width: 400
    height: 350

    modal: true

    property int chapterNo: 1
    property int verseNo: 1


    function run(place) {
        contentItem.state = "bookSelection"

        open()

        bookList.currentIndex = settings.translation.bookCodes().indexOf(placeAccesser.bookCode(place))
        bookList.positionViewAtIndex(bookList.currentIndex, GridView.Center)

        chapterGrid.selected = placeAccesser.chapterNo(place)
    }


    function place() {
        var bookCode = "xxx"
        if (bookList.currentIndex !== -1)
            bookCode = settings.translation.bookCodeAt(bookList.currentIndex)

        if (verseGrid.selected === 1)
            return placeAccesser.placeNoVerses(bookCode, chapterGrid.selected)
        else
            return placeAccesser.placeOneVerse(bookCode, chapterGrid.selected, verseGrid.selected)
    }

    Item {
        id: contentItem
        width: parent.width
        height: dialog.availableHeight - dialog.implicitFooterHeight - dialog.implicitHeaderHeight

        clip: true

        GridView {
            id: bookList

            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.topMargin: 3
            anchors.bottomMargin: 3

            width: parent.width

            cellWidth: width / Math.floor(width / 70)
            cellHeight: cellWidth * 0.75

            clip: true

            model: settings.translation

            cacheBuffer: 70

            layoutDirection: settings.translation.rtl ? Qt.RightToLeft : Qt.LeftToRight

            delegate: BookGridDelegate {
                width: bookList.cellWidth
                height: bookList.cellHeight

                bookColor: BookColors.bookColor(bookCode)

                // rtl: settings.translation.rtl

                onClicked: {
                    contentItem.state = "chapterSelection"
                }
            }


            onCurrentIndexChanged: {
                var bookCode = bookList.model.bookCodeAt(currentIndex)

                chapterGrid.size = bookList.model.chaptersInBook(bookCode)
                chapterGrid.selected = 1
            }
        }


        Item {
            id: secondPage

            anchors.left: bookList.right
            width: parent.width


            anchors.top: parent.top
            anchors.bottom: parent.bottom


            Flickable {
                id: chapterVerseFlickable

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.bottom: spacer.top

                contentHeight: verseGrid.y + verseGrid.height

                flickableDirection: Flickable.VerticalFlick

                clip: true


                Label {
                    id: chapterLabel

                    text: qsTr("Chapter:")

                    color: "#fff"
                }

                NumberGridSelector {
                    id: chapterGrid

                    anchors.top: chapterLabel.bottom
                    width: parent.width

                    size: 10

                    rtl: settings.translation.rtl

                    onSelectedChanged: {
                        verseGrid.size = bookList.model.versesInChapter(
                            bookList.model.bookCodeAt(bookList.currentIndex),
                            chapterGrid.selected
                        )
                        verseGrid.selected = 1
                    }
                }

                Label {
                    id: verseLabel

                    anchors.top: chapterGrid.bottom
                    anchors.topMargin: 10

                    text: qsTr("Verse:")

                    color: "#fff"
                }

                NumberGridSelector {
                    id: verseGrid

                    anchors.top: verseLabel.bottom
                    width: parent.width

                    size: 20

                    rtl: settings.translation.rtl
                }
            }


            Rectangle {
                id: spacer

                height: 10

                visible: false

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: backButton.top
            }


            Button {
                id: backButton

                anchors.left: parent.left
                width: parent.width / 4
                anchors.bottom: parent.bottom

                text: "←"

                onClicked: contentItem.state = "bookSelection"
            }

            Button {
                id: acceptButton

                anchors.left: backButton.right
                anchors.leftMargin: 20
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                text: qsTr("OK")

                onClicked: dialog.accept()
            }
        }


        state: "bookSelection"

        states: [
            State {
                name: "bookSelection"

                AnchorChanges {
                    target: bookList
                    anchors.left: contentItem.left
                }
            },
            State {
                name: "chapterSelection"

                AnchorChanges {
                    target: bookList
                    anchors.right: contentItem.left
                }
            }
        ]

        transitions: Transition {
            AnchorAnimation { duration: 100 }
        }
    }
}
