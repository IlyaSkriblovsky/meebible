import QtQuick 1.1
import com.meego 1.0

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

        anchors.verticalCenter: parent.verticalCenter
    }

    Switch {
        id: _switch

        anchors.right: parent.right
        anchors.rightMargin: 10

        anchors.verticalCenter: parent.verticalCenter
    }
}
