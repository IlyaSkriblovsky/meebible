import Qt 4.7
import com.nokia.symbian 1.1

Dialog {
    id: dialog

    property alias titleText: titleLabel.text

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

    // buttons: [
    //     Button {
    //         text: "Close"
    //         onClicked: dialog.reject()
    //     }
    // ]
}
