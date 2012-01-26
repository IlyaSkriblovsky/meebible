import Qt 4.7

Rectangle {
    id: button

    property alias text: label.text

    signal clicked()

    color: mouseArea.pressed ? '#8080ff' : '#c0c0ff'
    radius: 20

    Text {
        id: label
        anchors.centerIn: parent
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent

        onClicked: button.clicked()
    }
}
