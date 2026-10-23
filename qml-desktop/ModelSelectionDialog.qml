import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

UniDialog {
    id: dialog

    property alias model: listView.model

    // property alias header: header.children

    property int selectedIndex: -1


    contentItem: Item {
        id: contentItem
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter


        property int maximumContentHeight: 500
        height: Math.min(header.height + listView.contentHeight, maximumContentHeight)

        Binding {
            when: SYMBIAN

            target: contentItem
            property: "maximumContentHeight"
            value: dialog.platformContentMaximumHeight
        }

        Binding {
            when: ! SYMBIAN

            target: contentItem
            property: "maximumContentHeight"
            value: (screen.currentOrientation == Screen.Portrait ?
                screen.displayWidth * 0.8
                : screen.displayHeight * 0.8)
        }



        Item {
            id: header

            width: parent.width
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
    }
}
