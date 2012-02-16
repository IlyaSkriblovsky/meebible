import QtQuick 1.1
import com.meego 1.0

Image {
    property alias titleText: notice.titleText
    property alias message: notice.message

    source: theme.inverted ? 'image://theme/icon-s-description-inverse' : 'image://theme/icon-s-description'

    opacity: mouseArea.pressed ? 0.5 : 1.0

    QueryDialog {
        id: notice

        acceptButtonText: qsTr("OK")
    }

    MouseArea {
        id: mouseArea
        anchors.fill: parent

        onClicked: notice.open()
    }
}
