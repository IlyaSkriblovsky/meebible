import QtQuick 1.1
import com.meego 1.0

Image {
    property alias titleText: notice.titleText
    property alias message: notice.message

    source: mouseArea.pressed ? 'image://theme/icon-s-description-inverse' : 'image://theme/icon-s-description'

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
