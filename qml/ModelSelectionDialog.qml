import QtQuick 1.1

// This is for CommonDialog
import "/usr/lib/qt4/imports/com/meego/" 1.0


CommonDialog {
    id: dialog

    property alias listHeight: contentItem.height
    property alias model: listView.model

    property int selectedIndex: -1


    content: Item {
        id: contentItem
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter

        height: dialog.parent.height * 0.8
//        height: listView.height < dialog.parent.height * 0.8 ? listView.height : dialog.parent.height * 0.8

        ListView {
            id: listView

            anchors.fill: parent

            clip: true

            cacheBuffer: 70

            delegate: SimpleListDelegate {
                onClicked: {
                    selectedIndex = index
                    dialog.accept()
                }
            }
        }

        ScrollDecorator { flickableItem: listView }
    }
}
