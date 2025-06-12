import QtQuick 1.1

import com.nokia.symbian 1.1

import ".."

Page {
    id: page

    property alias headerText: header.text
    property alias content: contentWrapper.children
    property alias headerComponents: header.children

    signal closed()

    function openSheet(pageStack) { pageStack.push(this) }
    function closeSheet() { pageStack.pop(); closed() }


    Header {
        id: header
    }


    Item {
        id: contentWrapper

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.bottom: parent.bottom
    }


    tools: ToolBarLayout {
        UniToolIcon {
            platformIconId: "toolbar-back"
            onClicked: closeSheet()
        }
    }
}
