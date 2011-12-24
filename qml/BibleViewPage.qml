import QtQuick 1.1
import com.meego 1.0
import com.meego.extras 1.0
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
        property: "lineSpacing"
        value: lineSpacingSlider.value
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
        lineSpacingSlider.value = settings.lineSpacing
        created = true
    }


    Header {
        id: header
        text: bibleView.title
        withIcon: true
        height: settings.floatingHeader ? 0 : 70
        visible: ! settings.floatingHeader

        rtl: settings.translation.rtl
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
                duration: 500
            }

            enabled: bibleView.searchMode
        }

        Column {
            id: column

            Header {
                text: bibleView.title
                withIcon: true
                height: settings.floatingHeader ? 70 : 0
                visible: settings.floatingHeader

                rtl: settings.translation.rtl
            }

            BibleView {
                id: bibleView

                width: page.width

                resizesToContents: true
                preferredWidth: page.width

                url: "about:blank"

                translation: settings.translation

                inverted: theme.inverted


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

        color: theme.inverted ? '#000' : '#fff'

        BusyIndicator {
            id: realBusyIndicator

            anchors.centerIn: parent

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
                PropertyChanges {
                    target: realBusyIndicator
                    running: false
                }
            },
            State {
                name: "visible"

                PropertyChanges {
                    target: busyIndicator
                    opacity: 0.8
                }
                PropertyChanges {
                    target: realBusyIndicator
                    running: true
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



    QueryDialog {
        id: searchNotice

        titleText: qsTr("Please notice")

        message: qsTr("Full-text search works only for downloaded chapters. If you want to search whole Bible you must download it first using the app menu.")

        acceptButtonText: qsTr("OK")
    }


    Loader {
        id: searchDialog

        function load() { source = "SearchDialog.qml" }
        function open() {
            load()
            item.open()
            if (! settings.searchNoticeShown)
            {
                searchNotice.open()
                settings.searchNoticeShown = true
            }
        }

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
            text: qsTr("OK")

            anchors.verticalCenter: parent.verticalCenter

            anchors.right: parent.right
            anchors.rightMargin: 10
            width: 70

            onClicked: page.pageStack.toolBar.setTools(commonTools, "replace")
        }
    }


    ToolBarLayout {
        id: lineSpacingTools

        visible: false


        ToolButton {
            id: lineSpacingSmaller
            text: "–"

            anchors.left: parent.left
            anchors.leftMargin: 10
            width: 50

            onClicked: lineSpacingSlider.value -= 0.2
        }


        Slider {
            id: lineSpacingSlider
            minimumValue: 0.8
            maximumValue: 2
            stepSize: 0.1

            valueIndicatorVisible: true

            anchors.left: lineSpacingSmaller.right
            anchors.right: lineSpacingBigger.left

            onValueChanged: {
                var factor = parseFloat(value) / bibleView.lineSpacing
                bibleView.lineSpacing = value
                flickable.contentY *= factor
            }
        }

        ToolButton {
            id: lineSpacingBigger
            text: "+"

            anchors.right: lineSpacingDoneButton.left
            anchors.rightMargin: 10

            anchors.verticalCenter: parent.verticalCenter
            width: 50

            onClicked: lineSpacingSlider.value += 0.2
        }

        ToolButton {
            id: lineSpacingDoneButton
            text: qsTr("OK")

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
            text: qsTr("Done")

            anchors.verticalCenter: parent.verticalCenter
            anchors.rightMargin: 20

            onClicked: bibleView.stopSearchMode()
        }
    }


    Loader {
        id: fetcherDialog

        width: 10; height: 10

        function load() { source = "FetcherDialog.qml" }
        function open() { load(); item.open() }

        function start() { load(); item.start() }
    }

    QueryDialog {
        id: downloadConfirmDialog

        titleText: qsTr("Are you ready to download whole Bible?")
        message: qsTr("This may take 10-15 minutes and you'd better do it over Wi-Fi connection")

        acceptButtonText: qsTr("Download")
        rejectButtonText: qsTr("Cancel")

        onAccepted: fetcherDialog.start()
    }


    SettingsPage {
        id: settingsPage
    }

    AboutPage {
        id: aboutPage
    }


    InfoBanner { id: copyBanner }


    Menu {
        id: menu

        visualParent: pageStack

        MenuLayout {
            MenuItem {
                text: qsTr("Copy selected verses")
                onClicked: {
                    if (bibleView.copySelectedVerses())
                    {
                        copyBanner.text = qsTr("Copied")
                        bibleView.clearSelection()
                    }
                    else
                        copyBanner.text = qsTr("Nothing selected")
                    copyBanner.show()
                }
            }

            MenuItem {
                text: qsTr("Font size")
                onClicked: page.pageStack.toolBar.setTools(fontTools, "replace")
            }

            MenuItem {
                text: qsTr("Line spacing")
                onClicked: page.pageStack.toolBar.setTools(lineSpacingTools, "replace")
            }

            MenuItem {
                text: qsTr("Download Bible")
                onClicked: downloadConfirmDialog.open()
            }

            MenuItem {
                text: qsTr("Settings")
                onClicked: pageStack.push(settingsPage)
            }

            MenuItem {
                text: qsTr("About MeeBible")
                onClicked: pageStack.push(aboutPage)
            }
        }
    }
}
