import QtQuick 1.1
import com.nokia.meego 1.0
import MeeBible 0.1

Page {
    id: page



    property bool created: false
    property bool firstScrollPosSet: false

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
    Binding {
        target: settings
        property: "fontSize"
        value: fontSizeSlider.value
        when: created
    }
    Binding {
        target: settings
        property: "scrollPos"
        value: flickable.contentY
        when: firstScrollPosSet
    }
    Component.onCompleted: {
        bibleView.bookCode = settings.bookCode
        bibleView.chapterNo = settings.chapterNo
        fontSizeSlider.value = settings.fontSize
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

        flickableDirection: Flickable.VerticalFlick

        clip: true

        // This applies only to setting contentY programmaticaly
        Behavior on contentY {
            SmoothedAnimation {
                velocity: 5000
            }

            enabled: bibleView.searchMode
        }

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

                // fontSize: fontSizeSlider.value



                Component.onCompleted: loadChapter()

                onChapterLoaded: {
                    if (! firstScrollPosSet)
                    {
                        flickable.contentY = settings.scrollPos
                        firstScrollPosSet = true
                    }

                    page.state = "normal"
                }

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
                    var gap = 50
                    var flickY = y + bibleView.y
                    if (
                        flickY > flickable.contentY + gap &&
                        flickY < flickable.contentY + flickable.height - bibleView.fontSize
                       )
                        return

                    scrollTo(flickY - gap)
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

        function open() { load(); item.open(bibleView.bookCode, bibleView.chapterNo) }

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



    Connections {
        target: bibleView
        onSearchModeChanged: {
            if (bibleView.searchMode)
                page.pageStack.toolBar.setTools(searchTools, "set")
            else
                if (! fontTools.visible)
                    page.pageStack.toolBar.setTools(commonTools, "replace")
        }
    }



    tools: ToolBarLayout {
        id: commonTools

        ToolIcon {
            platformIconId: "toolbar-previous"
            onClicked: bibleView.loadPrevChapter()
        }

        ToolIcon {
            platformIconId: "toolbar-list"
            onClicked: placeDialog.open()
        }

        ToolIcon {
            platformIconId: "toolbar-next"
            onClicked: bibleView.loadNextChapter()
        }


        ToolIcon {
            platformIconId: "toolbar-search"
            onClicked: searchDialog.open()
        }

        ToolIcon {
            platformIconId: "toolbar-select-text"
            onClicked: page.pageStack.toolBar.setTools(fontTools, "replace")
        }

        ToolIcon {
            platformIconId: "toolbar-view-menu"
            onClicked: menu.status == DialogStatus.Closed ? menu.open() : menu.close()
        }
    }


    ToolBarLayout {
        id: fontTools

        visible: false


        ToolButton {
            id: fontSizeSmaller
            text: "–"

            anchors.left: parent.left
            anchors.leftMargin: 10
            width: 50

            onClicked: fontSizeSlider.value -= 2
        }


        Slider {
            id: fontSizeSlider
            minimumValue: 16
            maximumValue: 60
            stepSize: 1.0

            valueIndicatorVisible: true

            anchors.left: fontSizeSmaller.right
            anchors.right: fontSizeBigger.left

            onValueChanged: {
                var factor = parseFloat(value) / bibleView.fontSize
                bibleView.fontSize = value
                flickable.contentY *= factor * factor
            }
        }

        ToolButton {
            id: fontSizeBigger
            text: "+"

            anchors.right: fontDoneButton.left
            anchors.rightMargin: 10

            anchors.verticalCenter: parent.verticalCenter
            width: 50

            onClicked: fontSizeSlider.value += 2
        }

        ToolButton {
            id: fontDoneButton
            text: "OK"

            anchors.verticalCenter: parent.verticalCenter

            anchors.right: parent.right
            anchors.rightMargin: 10
            width: 70

            onClicked: page.pageStack.toolBar.setTools(commonTools, "replace")
        }
    }


    ToolBarLayout {
        id: searchTools

        visible: false

        ToolIcon {
            platformIconId: "toolbar-up"

            onClicked: {
                if (bibleView.matchIndex > 0)
                    bibleView.matchIndex = bibleView.matchIndex - 1
                else
                    bibleView.matchIndex = bibleView.matchCount - 1
            }
        }

        Label {
            id: matchCountLabel

            anchors.verticalCenter: parent.verticalCenter

            text: (bibleView.matchIndex + 1) + ' / ' + bibleView.matchCount

            font.pixelSize: 30
        }

        ToolIcon {
            platformIconId: "toolbar-down"

            onClicked: {
                if (bibleView.matchIndex < bibleView.matchCount - 1)
                    bibleView.matchIndex = bibleView.matchIndex + 1
                else
                    bibleView.matchIndex = 0
            }
        }

        ToolIcon {
            platformIconId: "toolbar-search"

            onClicked: searchDialog.open()
        }

        ToolButton {
            text: "Done"

            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 20

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
                text: window.showStatusBar ? "Fullscreen" : "Unfullscreen"
                onClicked: window.showStatusBar = ! window.showStatusBar
            }

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
