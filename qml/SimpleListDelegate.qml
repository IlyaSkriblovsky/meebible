import QtQuick 1.1
import com.nokia.meego 1.0

Item {
    id: item

    property bool centered: false
    property bool selectable: false

    height: 60
    width: parent.width

    signal clicked()

    Rectangle {
        anchors.fill: parent

        color: '#444'
        visible: mouseArea.pressed || (selectable && item.ListView.isCurrentItem)
    }

    Label {
        id: label

        anchors.verticalCenter: parent.verticalCenter

        text: value
        color: 'white'

        state: item.centered ? "centered" : ""

        states: [
            State {
                name: "centered"
                AnchorChanges {
                    target: label
                    anchors.horizontalCenter: item.horizontalCenter
                }
            }
        ]
    }

    MouseArea {
        id: mouseArea

        anchors.fill: parent

        onClicked: {
            item.clicked()
            if (item.selectable)
                item.ListView.view.currentIndex = index
        }
    }
}
