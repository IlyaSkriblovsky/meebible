import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Item {
    id: item

    property string title: model.title
    property string subtitle: model.subtitle
    property alias titleFontSize: title.font.pixelSize

    signal clicked
    signal pressAndHold

    property bool darkMode: Application.styleHints.colorScheme === Qt.ColorScheme.Dark

    height: 70

    clip: true

    Rectangle {
        anchors.fill: parent
        visible: mouseArea.pressed

        color: SYMBIAN ? "#288de0" : (darkMode ? "#ffffff" : "#221f1f")
        opacity: SYMBIAN ? 0.8 : (darkMode ? 0.2 : 0.1)
    }

    Column {
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.right: parent.right
        spacing: 5

        Label {
            id: title
            text: item.title
            font.bold: true
            font.pixelSize: 18
            color: (mouseArea.pressed && !SYMBIAN) ? "#797979" : (darkMode ? "#ffffff" : "#282828")
        }

        Label {
            id: text
            text: item.subtitle
            font.family: "Nokia Pure Text Light"
            font.pixelSize: 16
            color: (mouseArea.pressed && !SYMBIAN) ? "#797979" : (darkMode ? "#c8c8c8" : "#505050")

            visible: text !== ""

            width: parent.width
            elide: Text.ElideRight
            wrapMode: Text.NoWrap
        }
    }

    Rectangle {
        color: darkMode ? "#464646" : "#d4d4d4"
        width: parent.width
        height: 1
        anchors.bottom: parent.bottom
    }


    MouseArea {
        id: mouseArea

        anchors.fill: parent

        onClicked: item.clicked()
        onPressAndHold: item.pressAndHold()
    }
}
