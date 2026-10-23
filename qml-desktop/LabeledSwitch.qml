import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


Item {
    property alias text: _label.text
    property alias checked: _switch.checked


    anchors.left: parent.left
    anchors.right: parent.right

    height: 65

    Label {
        id: _label
        font.bold: true

        anchors.left: parent.left
        anchors.leftMargin: 10
        anchors.right: _switch.left
        anchors.rightMargin: 10

        anchors.verticalCenter: parent.verticalCenter

        wrapMode: Text.Wrap
    }

    Switch {
        id: _switch

        anchors.right: parent.right
        anchors.rightMargin: 10

        anchors.verticalCenter: parent.verticalCenter
    }
}
