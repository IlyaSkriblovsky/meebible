import QtQuick 1.1

import com.meego 1.0

Sheet {
    id: sheet


    property alias headerText: header.text
    property alias rejectButtonText: rejectButton.text
    property alias headerComponents: headerItem.children


    function openSheet() { open() }
    function closeSheet() { close() }


    buttons: [
        Item {
            id: headerItem

            anchors.left: parent.left
            anchors.right: rejectButton.left
            anchors.top: parent.top
            anchors.bottom: parent.bottom

            Label {
                id: header

                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 20

                font.pixelSize: 32
                font.family: "Nokia Pure Text Light"
                font.bold: false
            }
        },

        SheetButton {
            id: rejectButton

            text: qsTr("Close")

            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right
            anchors.rightMargin: 20

            onClicked: sheet.reject()
        }
    ]
}
