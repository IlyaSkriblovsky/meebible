import QtQuick 1.1
import com.meego 1.0
import MeeBible 0.1

Page {
    id: page



    property bool created: false

    Binding {
        target: settings
        property: "bookCode"
        value: bibleView.bookCode
        when: created
    }
    Binding {
        target: settings
        property: "chapterNo"
        value: bibleView.chapterNo
        when: created
    }
    Component.onCompleted: {
        bibleView.bookCode = settings.bookCode
        bibleView.chapterNo = settings.chapterNo
        created = true
    }


    Header {
        id: header
        text: bibleView.title
        height: settings.floatingHeader ? 0 : 70
        visible: ! settings.floatingHeader
    }


    Flickable {
        id: flickable

        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        contentWidth: column.width
        contentHeight:  column.height

        clip: true

        Column {
            id: column

            Header {
                text: bibleView.title
                height: settings.floatingHeader ? 70 : 0
                visible: settings.floatingHeader
            }

            BibleView {
                id: bibleView

                width: page.width

                resizesToContents: true
                preferredWidth: page.width

                url: "about:blank"

                translation: settings.translation



                Component.onCompleted: loadChapter()

                onChapterLoaded: { flickable.contentY = 0; page.state = "normal" }
                onChapterLoadingError: { flickable.contentY = 0; page.state = "normal" }
                onLoading: page.state = "loading"


                function scrollTo(y)
                {
                    var maxY = flickable.contentHeight - flickable.height
                    if (y < maxY)
                        flickable.contentY = y
                    else
                        if (maxY < 0)
                            flickable.contentY = 0
                        else
                            flickable.contentY = maxY
                }

                onNeedToScroll: {
                    scrollTo(y)
                }

                onEnsureVisible: {
                    var gap = 30
                    if (y > flickable.contentY && y < flickable.contentY + flickable.height - 30)
                        return

                    scrollTo(y)
                }
            }
        }
    }
    ScrollDecorator { flickableItem: flickable }


    Rectangle {
        id: busyIndicator

        anchors.fill: parent

        color: '#fff'

        BusyIndicator {
            anchors.centerIn: parent

            running: true

            platformStyle: BusyIndicatorStyle {
                size: "large"
            }
        }

        state: "invisible"

        states: [
            State {
                name: "invisible"

                PropertyChanges {
                    target: busyIndicator
                    opacity: 0.0
                }
            },
            State {
                name: "visible"

                PropertyChanges {
                    target: busyIndicator
                    opacity: 0.7
                }
            }
        ]

        transitions: Transition {
            NumberAnimation {
                properties: "opacity"
                duration: 100
            }
        }
    }

    state: "normal"

    states: [
        State {
            name: "normal"

            PropertyChanges {
                target: busyIndicator
                state: "invisible"
            }
        },
        State {
            name: "loading"

            PropertyChanges {
                target: busyIndicator
                state: "visible"
            }
        }
    ]



    Loader {
        id: placeDialog

        width: 10; height: 10

        function load() { source = "PlaceDialog.qml" }

        function open() { load(); item.open() }

        Connections {
            target: placeDialog.item
            onAccepted: bibleView.setAndLoad(placeDialog.item.bookCode(), placeDialog.item.chapterNo(), placeDialog.item.verseNo())
        }
    }



    Loader {
        id: searchDialog

        function load() { source = "SearchDialog.qml" }
        function open() { load(); item.open() }

        Connections {
            target: searchDialog.item
            onPlaceSelected: {
                searchDialog.item.close()
                bibleView.setAndLoad(bookCode, chapterNo, 1)
                bibleView.startSearchMode(searchDialog.item.query)
            }
        }
    }



    tools: commonTools


    ToolBarLayout {
        id: commonTools

        ToolIcon {
            platformIconId: "toolbar-previous"

            visible: ! bibleView.searchMode

            onClicked: bibleView.loadPrevChapter()
        }

        ToolIcon {
            platformIconId: "toolbar-next"

            visible: ! bibleView.searchMode

            onClicked: bibleView.loadNextChapter()
        }

        ToolIcon {
            platformIconId: "toolbar-update"

            visible: ! bibleView.searchMode

            onClicked: placeDialog.open()
        }

        ToolIcon {
            platformIconId: "toolbar-search"

            visible: ! bibleView.searchMode

            onClicked: searchDialog.open()
        }

        ToolIcon {
            platformIconId: "toolbar-view-menu"

            visible: ! bibleView.searchMode

            onClicked: menu.status == DialogStatus.Closed ? menu.open() : menu.close()
        }

        ToolIcon {
            platformIconId: "toolbar-up"

            visible: bibleView.searchMode

            onClicked: {
                if (bibleView.matchIndex > 0)
                    bibleView.matchIndex = bibleView.matchIndex - 1
                else
                    bibleView.matchIndex = bibleView.matchCount - 1
            }
        }

        Label {
            text: (bibleView.matchIndex + 1) + ' / ' + bibleView.matchCount

            visible: bibleView.searchMode
        }

        ToolIcon {
            platformIconId: "toolbar-down"

            visible: bibleView.searchMode

            onClicked: {
                if (bibleView.matchIndex < bibleView.matchCount - 1)
                    bibleView.matchIndex = bibleView.matchIndex + 1
                else
                    bibleView.matchIndex = 0
            }
        }

        ToolButton {
            text: "Done"

            visible: bibleView.searchMode

            onClicked: bibleView.stopSearchMode()
        }
    }


    SettingsPage {
        id: settingsPage
    }

    AboutPage {
        id: aboutPage
    }


    Menu {
        id: menu

        visualParent: pageStack

        MenuLayout {
            MenuItem {
                text: "Settings"
                onClicked: pageStack.push(settingsPage)
            }

            MenuItem {
                text: "About MeeBible"
                onClicked: pageStack.push(aboutPage)
            }
        }
    }
}
