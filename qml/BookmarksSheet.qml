import QtQuick 1.1

import com.meego 1.0
import com.meego.extras 1.0


Sheet {
    signal bookmarkSelected(variant place)


    id: dialog

    buttons: [
        Label {
            id: header

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.leftMargin: 10

            text: qsTr("Bookmarks")

            font.pixelSize: 32
            font.family: "Nokia Pure Text Light"
            font.bold: false
        },

        SheetButton {
            text: qsTr("Close")

            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 10

            onClicked: dialog.reject()
        }
    ]


    content: Item {
        id: item

        anchors.fill: parent


        ListView {
            id: list

            anchors.fill: parent

            clip: true

            cacheBuffer: 50

            model: bookmarks

            delegate: ListDelegate {
                x: 10
                width: parent.width - 20

                clip: true

                onClicked: {
                    dialog.bookmarkSelected(model.place)
                    dialog.accept()
                }
            }
        }
    }
}
