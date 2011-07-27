import QtQuick 1.0
import MeeBible 0.1

Rectangle {
    width: 500
    height: 500

    MouseArea {
        id: button

        anchors.left: parent.left
        anchors.top: parent.top
        anchors.right: parent.right
        height: 50
        width: parent.width

        onClicked: {
            console.log('qwe')
            bibleView.setTranslation(dflTranslation)
            bibleView.loadChapter("mt", 5)
        }

        Rectangle {
            anchors.fill: parent
            color: 'red'
        }
    }

    Rectangle {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: button.bottom
        anchors.bottom: parent.bottom

        clip: true

        Flickable {
            anchors.fill: parent

            contentWidth: bibleView.width
            contentHeight:  bibleView.height

            BibleView {
                id: bibleView
            }
        }
    }
}
