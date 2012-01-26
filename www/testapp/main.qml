import Qt 4.7

Item {
    width: 600
    height: 800

    Item {
        id: buttonRow

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top
        height: 60

        Button {
            anchors.fill: parent
            text: "reload"

            onClicked: {
                languages.reload()
                busy.show()
            }
        }
    }

    Connections {
        target: languages

        onLoaded: busy.hide()
    }

    ListView {
        anchors.left: parent.left
        anchors.top: buttonRow.bottom
        anchors.bottom: parent.bottom
        width: parent.width / 2

        model: languages

        clip: true

        header: Text {
            height: 50
            text: "Languages:"
        }

        delegate: Rectangle {
            width: parent.width
            height: 40
            color: '#c0c0c0'

            Text {
                anchors.fill: parent
                text: engname + ' (' + selfname + ')'
            }
        }
    }

    Rectangle {
        id: busy

        anchors.fill: parent

        color: '#000'

        opacity: 0.4

        visible: false

        function show() {
            visible = true
        }

        function hide() {
            visible = false
        }
    }
}
