import QtQuick 1.1
import com.meego 1.0

Item {
    id: item

    property alias buttonText: button.text
    property alias helpTitle: help.titleText
    property alias helpMessage: help.message

    signal clicked()


    anchors.left: parent.left
    anchors.right: parent.right

    height: 80

    Button {
        id: button

        anchors.centerIn: parent

        onClicked: item.clicked()
    }

    HelpButton {
        id: help

        anchors.right: parent.right
        anchors.rightMargin: 15
        anchors.verticalCenter: parent.verticalCenter
    }
}
