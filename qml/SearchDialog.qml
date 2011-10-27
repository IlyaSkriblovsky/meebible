import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1
import MeeBible 0.1

Sheet {
    id: dialog


    property QtObject translation: settings.translation

    property alias query: field.text

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
            anchors.rightMargin: 10

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
                    inverted: false
                }
            }

            state: cache.searchInProgress ? "busy" : "idle"

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

                results.clear()
            }
            onSearchFinished: {
                if (results.count == 0)
                    item.state = "nothing"
            }

            onMatchFound: {
                results.append({
                    title: translation.bookName(bookCode) + ' ' + chapterNo,
                    subtitle: match,
                    bookCode: bookCode,
                    chapterNo: chapterNo,
                    matchCount: matchCount
                })
            }
        }


        anchors.fill: parent



        ListView {
            id: list

            anchors.fill: parent

            clip: true

            cacheBuffer: 70

            model: ListModel {
                id: results
            }

            delegate: ListDelegate {
                onClicked: dialog.placeSelected(bookCode, chapterNo)

                clip: true

                x: 10
                width: parent.width - 20

                CountBubble {
                    value: matchCount
                    anchors.right: parent.right
                    anchors.top: parent.top
                    anchors.topMargin: 15
                }
            }
        }
        ScrollDecorator { flickableItem: list; __rightPageMargin: 30 }

        Item {
            id: nothing

            anchors.fill: parent

            Label {
                anchors.centerIn: parent

                text: "Nothing found"
                color: '#888'
                font.pixelSize: 60
                font.family: "Nokia Pure Text Light"
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
