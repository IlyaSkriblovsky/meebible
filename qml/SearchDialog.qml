import QtQuick 1.1
import com.meego 1.0
import com.meego.extras 1.0
import MeeBible 0.1

Sheet {
    id: dialog


    property QtObject translation: null

    signal placeSelected(string bookCode, int chapterNo)


    function startSearch() {
        if (field.text == "")
            return

        cache.search(translation, field.text)
    }


    buttons: [
        SheetButton {
            id: close
            text: "Close"

            anchors.verticalCenter: parent.verticalCenter
            anchors.right: parent.right

            onClicked: dialog.reject()
        },

        TextField {
            id: field

            placeholderText: "Search..."

            anchors.left: parent.left
            anchors.leftMargin: 10
            anchors.rightMargin: 10
            anchors.verticalCenter: parent.verticalCenter
            anchors.right: close.left


            platformSipAttributes: SipAttributes {
                actionKeyIcon: "/usr/share/themes/blanco/meegotouch/icons/icon-m-toolbar-search-selected.png"
                actionKeyEnabled: true
            }

            Keys.onReturnPressed: {
                platformCloseSoftwareInputPanel()
                dialog.startSearch()
            }


            Image {
                id: image

                source: "image://theme/icon-m-common-search"

                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 10

                width: 48
                height: 48


                MouseArea {
                    anchors.fill: parent

                    onClicked: dialog.startSearch()
                }
            }

            BusyIndicator {
                id: busyIndicator

                anchors.verticalCenter: parent.verticalCenter
                anchors.right: parent.right
                anchors.rightMargin: 15

                running: visible

                platformStyle: BusyIndicatorStyle {
                    size: "medium"
                }
            }

            state: "idle"

            states: [
                State {
                    name: "idle"

                    PropertyChanges {
                        target: image
                        visible: true
                    }

                    PropertyChanges {
                        target: busyIndicator
                        visible: false
                    }
                },
                State {
                    name: "busy"

                    PropertyChanges {
                        target: image
                        visible: false
                    }

                    PropertyChanges {
                        target: busyIndicator
                        visible: true
                    }
                }
            ]
        }
    ]

    content: Item {
        id: item


        Connections {
            target: cache

            onSearchStarted: {
                item.state = "list"
                field.state = "busy"

                results.clear()
            }
            onSearchFinished: {
                field.state = "idle"

                if (results.count == 0)
                    item.state = "nothing"
            }

            onMatchFound: {
                results.append({
                    title: translation.bookName(bookCode) + ' ' + chapterNo,
                    subtitle: match,
                    bookCode: bookCode,
                    chapterNo: chapterNo
                })
            }
        }


        anchors.fill: parent

        anchors.leftMargin: 10
        anchors.rightMargin: 10



        ListView {
            id: list

            anchors.fill: parent

            clip: true

            model: ListModel {
                id: results
            }

            delegate: ListDelegate {
                onClicked: dialog.placeSelected(bookCode, chapterNo)
            }
        }
        ScrollDecorator { flickableItem: list }

        Item {
            id: nothing

            anchors.fill: parent

            Label {
                anchors.centerIn: parent

                text: "Nothing found"
                color: '#666'
                font.pixelSize: 40
            }
        }


        state: "list"

        states: [
            State {
                name: "list"

                PropertyChanges {
                    target: list
                    visible: true
                }
                PropertyChanges {
                    target: nothing
                    visible: false
                }
            },
            State {
                name: "nothing"

                PropertyChanges {
                    target: list
                    visible: false
                }
                PropertyChanges {
                    target: nothing
                    visible: true
                }
            }
        ]
    }
}
