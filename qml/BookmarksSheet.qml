import QtQuick 1.1

import com.meego 1.0
import com.meego.extras 1.0


Sheet {
    signal bookmarkSelected(string bookCode, int chapterNo, variant verses)


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
                // title: value
                // subtitle: text

                onClicked: {
                    console.log('onClicked ' + model.bookCode + ', ' + model.chapterNo)
                    dialog.bookmarkSelected(model.bookCode, model.chapterNo, model.verses)
                }
            }
        }
    }
}
