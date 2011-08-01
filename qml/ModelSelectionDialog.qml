import QtQuick 1.1
import com.meego 1.0


Dialog {
    id: dialog

    property alias titleText: headerLabel.text
    property alias listHeight: contentItem.height
    property alias model: listView.model

    property int selectedIndex: -1


    title: Item {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: parent.top

        height: 44

        Label {
            id: headerLabel

            anchors.fill: parent

            color: 'white'

            font.pixelSize: 32
        }

        Image {
            id: closeButton
            anchors.bottom: headerLabel.bottom
            anchors.bottomMargin: 8
            anchors.right: parent.right

            opacity: closeButtonArea.pressed ? 0.5 : 1.0

            source: "image://theme/icon-m-common-dialog-close"

            MouseArea {
                id: closeButtonArea
                anchors.fill: parent
                onClicked: dialog.reject()
            }

        }

        Rectangle {
            id: headerLine

            anchors.left: parent.left
            anchors.right: parent.right

            anchors.bottom:  headerLabel.bottom

            height: 1

            color: "#4D4D4D"
        }
    }

    content: Item {
        id: contentItem
        width: parent.width
        anchors.horizontalCenter: parent.horizontalCenter

        height: dialog.parent.height * 0.8
//        height: listView.height < dialog.parent.height * 0.8 ? listView.height : dialog.parent.height * 0.8

        ListView {
            id: listView

            anchors.fill: parent

            clip: true

            cacheBuffer: 70

            delegate: Item {
                height: 60
                width: parent.width

                Rectangle {
                    anchors.fill: parent

                    color: '#444'
                    visible: mouseArea.pressed
                }

                Label {
                    width: parent.width
                    anchors.verticalCenter: parent.verticalCenter

                    text: value
                    color: 'white'
                }

                MouseArea {
                    id: mouseArea

                    anchors.fill: parent

                    onClicked: {
                        selectedIndex = index
                        dialog.accept()
                    }
                }
            }
        }

        ScrollDecorator { flickableItem: listView }
    }
}
