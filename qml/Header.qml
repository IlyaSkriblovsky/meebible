import QtQuick 1.1
import com.meego 1.0


Rectangle {
    id: header


    property alias text: label.text


    gradient: Gradient {
        GradientStop { position: 0.0; color: "#515151" }
        GradientStop { position: 1.0; color: "#313131" }
    }

    width: parent.width
    height: 70

    Label {
        x: label.x
        y: label.y + 1
        text: label.text
        font.pixelSize: 32
        color: '#000'
    }

    Label {
        id: label
        anchors.verticalCenter: parent.verticalCenter
        anchors.left: parent.left
        anchors.leftMargin: 10
        font.pixelSize: 32
        color: '#eee'
    }


    Rectangle {
        x: 0
        y: 68
        width: parent.width
        height: 1
        color: '#000' // Qt.darker('#313131', 1.2)
    }

    Rectangle {
        x: 0
        y: 69
        width: parent.width
        height: 1
        color: '#fff'
    }
}
