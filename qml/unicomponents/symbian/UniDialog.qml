import Qt 4.7
import com.nokia.symbian 1.1

Dialog {
    id: dialog

    property alias titleText: titleLabel.text

    property alias uniButtons: buttonsParent.children


    onClickedOutside: reject()

    title: [
        Item {
            anchors.fill: parent

            Label {
                id: titleLabel
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.leftMargin: 10

                font {
                    family: platformStyle.fontFamilyRegular
                    pixelSize: platformStyle.fontSizeLarge
                }
                color: platformStyle.colorNormalLink
            }

            Image {
                id: closeButton

                anchors.verticalCenter: titleLabel.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 10

                opacity: closeButtonMouseArea.pressed ? 0.5 : 1.0

                source: "icon-m-common-dialog-close.png"

                MouseArea {
                    id: closeButtonMouseArea
                    anchors.fill: parent
                    onClicked: dialog.reject()
                }
            }
        }
    ]

    buttons: Item {
        width: parent.width
        height: buttonsParent.children.length > 0 ? buttonsParent.height + 8 * 2 : 0

        Row {
            id: buttonsParent

            anchors.centerIn: parent

            spacing: 8
        }
    }
}
