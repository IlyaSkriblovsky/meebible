import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Image {
    id: root

    property alias titleText: notice.titleText

    property string message: ""

    source: settings.inverted ? "help-button-inverted.png" : "help-button.png"

    opacity: mouseArea.pressed ? 0.5 : 1.0

    QueryDialog {
        id: notice

        message: root.message + (SYMBIAN ? "\n" : "")

        // acceptButtonText: qsTr("OK")
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: notice.open()
    }
}
