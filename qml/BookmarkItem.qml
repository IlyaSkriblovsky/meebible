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

    BorderImage {
        anchors.fill: parent
        // Fill page porders
        visible: mouseArea.pressed
        source: theme.inverted ? "image://theme/meegotouch-panel-inverted-background-pressed" : "image://theme/meegotouch-panel-background-pressed"
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
            color: mouseArea.pressed ? "#797979" : "#282828"
        }

        Label {
            id: text
            text: item.subtitle
            font.family: "Nokia Pure Text Light"
            font.pixelSize: 22
            color: mouseArea.pressed ? "#797979" : "#505050"

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
