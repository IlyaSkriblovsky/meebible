import QtQuick 1.1

import com.meego 1.0

Item {
    id: item


    property string title: model.title
    property string subtitle: model.subtitle


    signal clicked
    signal pressAndHold


    height: 80


    clip: true


    Rectangle {
        anchors.fill: parent
        visible: mouseArea.pressed

        color: settings.inverted ? "#ffffff" : "#221f1f"
        opacity: settings.inverted ? 0.2 : 0.1
    }

    Column {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right

        Label {
            id: title
            text: item.title
            font.bold: true
            font.pixelSize: 26
            color: mouseArea.pressed ? "#797979" : (settings.inverted ? "#ffffff" : "#282828")
        }

        Label {
            id: text
            text: item.subtitle
            font.family: "Nokia Pure Text Light"
            font.pixelSize: 22
            color: mouseArea.pressed ? "#797979" : (settings.inverted ? "#c8c8c8" : "#505050")

            visible: text != ""

            width: parent.width
            elide: Text.ElideRight
        }
    }


    MouseArea {
        id: mouseArea

        anchors.fill: parent

        onClicked: item.clicked()
        onPressAndHold: item.pressAndHold()
    }
}
