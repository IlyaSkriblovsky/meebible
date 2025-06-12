import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 2.15


Page {
    id: dialog

    signal bookmarkSelected(variant place)


    function closeSheet() {
        if (dialog.StackView.view) {
            dialog.StackView.view.pop()
        }
    }


    // headerText: qsTr("Bookmarks")
    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 10

            ToolButton {
                text: '< Back'
                onClicked: {
                    closeSheet()
                }
            }

            Label {
                text: qsTr("Bookmarks")
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }


    Item {
        id: item

        anchors.fill: parent

        ListView {
            id: list

            anchors.fill: parent
            leftMargin: 12
            rightMargin: 12

            clip: true

            cacheBuffer: 50

            model: bookmarks

            delegate: SubtitledListDelegate {
                width: list.width - 24

                onClicked: {
                    dialog.bookmarkSelected(model.place)
                    dialog.closeSheet()
                }

                onPressAndHold: {
                    dialog._pressAndHoldIndex = index
                    itemMenu.popup(this)
                }
            }
        }


        Label {
            visible: list.count === 0

            anchors.fill: parent
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.Wrap

            text: qsTr('No bookmarks yet')
            color: Application.styleHints.colorScheme === Qt.ColorScheme.Dark ? '#444' : '#888'
            font.pixelSize: SYMBIAN ? 48 : 60
            font.family: 'Nokia Pure Text Light'
        }
    }



    property int _pressAndHoldIndex: -1

    Menu {
        id: itemMenu

        MenuItem {
            text: qsTr("Delete")

            onClicked: {
                bookmarks.deleteBookmark(dialog._pressAndHoldIndex)
            }
        }
    }
}
