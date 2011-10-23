import QtQuick 1.1

// This is for CommonDialog
import "/usr/lib/qt4/imports/com/nokia/meego/" 1.0


CommonDialog {
    id: dialog

    titleText: "Select Book"

    property alias bookModel: bookList.model


    function open() {
        contentItem.state = "bookSelection"
        if (status == DialogStatus.Closed)
            status = DialogStatus.Opening;
    }


    function bookCode() {
        if (bookList.currentIndex == -1)
            return "xxx"

        return bookModel.bookCodeAt(bookList.currentIndex)
    }

    function bookName() {
        return bookModel.bookName(bookCode())
    }

    function chapterNo() {
        return chaptersList.currentIndex + 1
    }

    function verseNo() {
        return versesList.currentIndex + 1
    }

    content: Item {
        id: contentItem
        width: parent.width

        anchors.horizontalCenter: parent.horizontalCenter

        height: dialog.parent.height * 0.8

        clip: true

        ListView {
            id: bookList

            anchors.top: parent.top
            anchors.bottom: parent.bottom

            width: parent.width

            clip: true

            model: settings.translation

            cacheBuffer: 70

            delegate: SimpleListDelegate {
                selectable: true

                onClicked: {
                    chaptersModel.clear()
                    var chaptersCount = bookList.model.chaptersInBook(bookList.model.bookCodeAt(index))
                    for (var i = 1; i <= chaptersCount; i++)
                        chaptersModel.append({ value: i })


                    chaptersList.currentIndex = 0

                    contentItem.state = "chapterSelection"
                }
            }
        }
        ScrollDecorator { flickableItem: bookList }


        Item {
            anchors.left: bookList.right
            width: parent.width
            height: parent.height



            Item {
                id: chapterLabel

                height: _chapterLabel.height

                anchors.left: parent.left
                anchors.right: parent.horizontalCenter

                Label {
                    id: _chapterLabel

                    anchors.horizontalCenter: parent.horizontalCenter

                    text: "Chapter:"
                    color: "white"
                }
            }
            Item {
                id: verseLabel

                height: _verseLabel.height

                anchors.left: parent.horizontalCenter
                anchors.right: parent.right

                Label {
                    id: _verseLabel

                    anchors.horizontalCenter: parent.horizontalCenter

                    text: "Verse:"
                    color: "white"
                }
            }


            ListView {
                id: chaptersList

                anchors.left: parent.left
                anchors.right: parent.horizontalCenter
                anchors.top: chapterLabel.bottom
                anchors.bottom: backButton.top

                model: ListModel {
                    id: chaptersModel
                }

                clip: true
                cacheBuffer: 70

                delegate: SimpleListDelegate {
                    centered: true
                    selectable: true
                }

                onCurrentIndexChanged: {
                    versesModel.clear()
                    if (currentIndex == -1)
                        return

                    console.log('bookCode=' + bookList.model.bookCodeAt(bookList.currentIndex))
                    console.log('currentIndex=' + currentIndex)

                    var versesCount = bookList.model.versesInChapter(
                        bookList.model.bookCodeAt(bookList.currentIndex),
                        currentIndex + 1
                    )
                    for (var i = 1; i <= versesCount; i++)
                        versesModel.append({ value: i })
                }
            }
            ScrollDecorator { flickableItem: chaptersList }

            ListView {
                id: versesList

                anchors.left: parent.horizontalCenter
                anchors.right: parent.right
                anchors.top: verseLabel.bottom
                anchors.bottom: acceptButton.top

                model: ListModel {
                    id: versesModel
                }

                clip: true
                cacheBuffer: 70

                delegate: SimpleListDelegate {
                    centered: true
                    selectable: true
                }
            }
            ScrollDecorator { flickableItem: versesList }



            id: buttonRow

            anchors.bottom: parent.bottom

            Button {
                id: backButton

                anchors.left: parent.left
                width: parent.width / 4
                anchors.bottom: parent.bottom

                platformStyle: ButtonStyle {
                    inverted: true
                }

                text: "←"

                onClicked: contentItem.state = "bookSelection"
            }

            Button {
                id: acceptButton

                anchors.left: backButton.right
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                platformStyle: ButtonStyle {
                    inverted: true
                }

                text: "OK"

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
            AnchorAnimation { duration: 200 }
        }
    }
}
