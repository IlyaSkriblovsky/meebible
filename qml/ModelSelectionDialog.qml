import QtQuick 1.1

// This is for CommonDialog
import "/usr/lib/qt4/imports/com/meego/" 1.0


CommonDialog {
    id: dialog

    property alias model: listView.model

    property alias header: header.children

    property int selectedIndex: -1


    content: Item {
        id: contentItem
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter

        height: Math.min(header.height + listView.contentHeight, dialog.parent.height * 0.7)

        Item {
            id: header

            height: childrenRect.height
        }

        ListView {
            id: listView

            anchors.top: header.bottom
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.bottom: parent.bottom

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
