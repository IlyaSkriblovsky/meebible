import Qt 4.7
import com.nokia.meego 1.1

Dialog {
    id: dialog


    property alias titleText: titleLabel.text

    property alias uniButtons: buttonsParent.children


    width: screen.currentOrientation == Screen.Portrait ? screen.displayHeight * 0.9 : screen.displayWidth * 0.8


    title: [
        Item {
            height: 56
            anchors.fill: parent

            Label {
                anchors.verticalCenter: parent.verticalCenter

                id: titleLabel
                text: "This is title"

                color: '#fff'
                font.pixelSize: 32
            }

            Image {
                id: closeButton
                anchors.verticalCenter: titleLabel.verticalCenter
                anchors.right: parent.right

                opacity: closeButtonMouseArea.pressed ? 0.5 : 1.0

                source: "image://theme/icon-m-common-dialog-close"

                MouseArea {
                    id: closeButtonMouseArea
                    anchors.fill: parent
                    onClicked: dialog.reject()
                }
            }

            Rectangle {
                id: headerLine

                anchors.left: parent.left
                anchors.right: parent.right
                anchors.bottom: parent.bottom

                height: 1
                color: "#4d4d4d"
            }
        }
    ]

    buttons: Item {
        width: parent.width

        height: buttonsParent.children.length > 0 ? buttonsParent.height + 25 : 0

        Column {
            id: buttonsParent

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom

            spacing: 10
        }
    }
}
