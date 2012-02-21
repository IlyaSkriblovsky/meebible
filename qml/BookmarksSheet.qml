import QtQuick 1.1

import com.meego 1.0
import com.meego.extras 1.0


Sheet {
    signal bookmarkSelected(variant place)


    id: dialog

    buttons: [
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
                onClicked: {
                    dialog.bookmarkSelected(model.place)
                    dialog.accept()
                }
            }
        }
    }
}
