import QtQuick 1.1
import com.nokia.meego 1.0
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
    Binding {
        target: settings
        property: "fontSize"
        value: fontSizeSlider.value
        when: created
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




    tools: ToolBarLayout {
        id: tools

        ToolIcon {
            id: prevButton

            platformIconId: "toolbar-previous"

            onClicked: bibleView.loadPrevChapter()
        }

        ToolIcon {
            id: nextButton

            platformIconId: "toolbar-next"

            onClicked: bibleView.loadNextChapter()
        }

        ToolIcon {
            id: placeButton

            platformIconId: "toolbar-list"

            onClicked: placeDialog.open()
        }

        ToolIcon {
            id: prevMatchButton

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
            id: nextMatchButton

            platformIconId: "toolbar-down"

            onClicked: {
                if (bibleView.matchIndex < bibleView.matchCount - 1)
                    bibleView.matchIndex = bibleView.matchIndex + 1
                else
                    bibleView.matchIndex = 0
            }
        }

        ToolIcon {
            id: searchButton

            platformIconId: "toolbar-search"

            onClicked: searchDialog.open()
        }

        ToolIcon {
            id: fontButton

            platformIconId: "toolbar-select-text"

            onClicked: tools.state = "fontSizeMode"
        }


        Slider {
            id: fontSizeSlider
            minimumValue: 16
            maximumValue: 70
            stepSize: 1.0

            anchors.left: parent.left
            anchors.right: doneButton.left

            onValueChanged: {
                var factor = parseFloat(value) / bibleView.fontSize
                bibleView.fontSize = value
                flickable.contentY *= factor * factor
            }
        }

        ToolButton {
            id: doneButton

            text: "Done"

            anchors.verticalCenter: parent.verticalCenter

            onClicked: {
                if (bibleView.searchMode)
                    bibleView.stopSearchMode()

                tools.state = "normal"
            }
        }

        ToolIcon {
            id: menuButton

            platformIconId: "toolbar-view-menu"

            onClicked: menu.status == DialogStatus.Closed ? menu.open() : menu.close()
        }

        state: "normal"


        Connections {
            target: bibleView
            onSearchModeChanged: {
                if (bibleView.searchMode)
                    tools.state = "searchMode"
            }
        }


        states: [
            State {
                name: "normal"

                PropertyChanges { target: prevButton;       visible: true; }
                PropertyChanges { target: nextButton;       visible: true; }
                PropertyChanges { target: placeButton;      visible: true; }
                PropertyChanges { target: prevMatchButton;  visible: false; }
                PropertyChanges { target: matchCountLabel;  visible: false; }
                PropertyChanges { target: nextMatchButton;  visible: false; }
                PropertyChanges { target: searchButton;     visible: true; }
                PropertyChanges { target: fontButton;       visible: true; }
                PropertyChanges { target: fontSizeSlider;   visible: false; }
                PropertyChanges { target: doneButton;       visible: false; }
                PropertyChanges { target: menuButton;       visible: true; }
            },
            State {
                name: "searchMode"

                PropertyChanges { target: prevButton;       visible: false; }
                PropertyChanges { target: nextButton;       visible: false; }
                PropertyChanges { target: placeButton;      visible: false; }
                PropertyChanges { target: prevMatchButton;  visible: true; }
                PropertyChanges { target: matchCountLabel;  visible: true; }
                PropertyChanges { target: nextMatchButton;  visible: true; }
                PropertyChanges { target: searchButton;     visible: true; }
                PropertyChanges { target: fontButton;       visible: false; }
                PropertyChanges { target: fontSizeSlider;   visible: false; }
                PropertyChanges { target: doneButton;       visible: true; }
                PropertyChanges { target: menuButton;       visible: false; }
            },
            State {
                name: "fontSizeMode"

                PropertyChanges { target: prevButton;       visible: false; }
                PropertyChanges { target: nextButton;       visible: false; }
                PropertyChanges { target: placeButton;      visible: false; }
                PropertyChanges { target: prevMatchButton;  visible: false; }
                PropertyChanges { target: matchCountLabel;  visible: false; }
                PropertyChanges { target: nextMatchButton;  visible: false; }
                PropertyChanges { target: searchButton;     visible: false; }
                PropertyChanges { target: fontButton;       visible: false; }
                PropertyChanges { target: fontSizeSlider;   visible: true; }
                PropertyChanges { target: doneButton;       visible: true; }
                PropertyChanges { target: menuButton;       visible: false; }
            }
        ]
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
