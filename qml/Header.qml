import QtQuick 1.1
import com.meego 1.0


Rectangle {
    id: header


    property alias text: label.text
    property bool withIcon: false
    property bool rtl: false


    gradient: Gradient {
        GradientStop { position: 0.0; color: "#515151" }
        GradientStop { position: 1.0; color: "#313131" }
    }

    width: parent.width
    height: 70

    Image {
        id: icon
        anchors.left: parent.left
        anchors.leftMargin: 3
        anchors.top: parent.top
        anchors.bottom: parent.bottom
        width: withIcon ? height : 0

        visible: withIcon

        source: withIcon ? "header.png" : ""
    }

    Label {
        x: label.x
        y: label.y + 1
        text: label.text
        font.pixelSize: 32
        color: '#000'

        font.family: "Nokia Pure Text Light"
    }

    Label {
        id: label
        anchors.verticalCenter: parent.verticalCenter
        anchors.leftMargin: withIcon ? 5 : 20
        anchors.rightMargin: 10
        font.pixelSize: 32
        color: '#eee'

        font.family: "Nokia Pure Text Light"

        state: header.rtl ? "rtl" : "ltr"

        states: [
            State {
                name: "rtl"
                AnchorChanges {
                    target: label
                    anchors.right: header.right
                }
            },

            State {
                name: "ltr"
                AnchorChanges {
                    target: label
                    anchors.left: icon.right
                }
            }
        ]
    }


    Rectangle {
        x: 0
        y: 68
        width: parent.width
        height: 1
        color: theme.inverted ? '#222' : '#000'
    }

    Rectangle {
        x: 0
        y: 69
        width: parent.width
        height: 1
        color: theme.inverted ? '#000' : '#fff'
    }
}
