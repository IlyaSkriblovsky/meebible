import QtQuick 1.1
import com.nokia.symbian 1.1

ListItem {
    id: item


    property string title: model.title
    property string subtitle: model.subtitle


    clip: true


    Column {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right

        Label {
            id: title
            text: item.title
            font.bold: true
            font.pixelSize: 26

            platformInverted: !settings.inverted
        }

        Label {
            id: text
            text: item.subtitle
            font.family: "Nokia Pure Text Light"
            font.pixelSize: 22

            visible: text != ""

            width: parent.width
            elide: Text.ElideRight

            platformInverted: !settings.inverted
        }
    }
}
