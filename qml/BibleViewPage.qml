import QtQuick 1.1

// for EditBubbleButton
import "/usr/lib/qt4/imports/com/meego/" 1.0

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
    Binding {
        target: settings
        property: 'inverted'
        value: theme.inverted
        when: created
    }
    Component.onCompleted: {
        bibleView.bookCode = settings.bookCode
        bibleView.chapterNo = settings.chapterNo
        fontSizeSlider.value = settings.fontSize
        lineSpacingSlider.value = settings.lineSpacing
        theme.inverted = settings.inverted
        created = true

        languages.reload(true)
    }


    Header {
        id: header
        text: bibleView.title || "MeeBible"
        withIcon: true
        height: settings.floatingHeader ? 0 : 70
        visible: ! settings.floatingHeader

        rtl: settings.translation != null  &&  settings.translation.rtl
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
                text: bibleView.title || "MeeBible"
                withIcon: true
                height: settings.floatingHeader ? 70 : 0
                visible: settings.floatingHeader

                rtl: settings.translation != null  &&  settings.translation.rtl
            }

            BibleView {
                id: bibleView

                width: page.width

                resizesToContents: true
                preferredWidth: page.width

                url: "about:blank"

                translation: settings.translation

                inverted: theme.inverted


                fontName: settings.fontName


                // Component.onCompleted: loadChapter()

                onChapterLoaded: {
                    if (! firstScrollPosSet)
                    {
                        flickable.contentY = settings.scrollPos
                        firstScrollPosSet = true
                    }

                    // page.state = "normal"
                }

                onChapterLoadingError: {
                    flickable.contentY = 0
                    // page.state = "normal"
                }
                // onLoading: page.state = "loading"


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



    InfoBanner {
        id: copyBanner
        text: qsTr("Copied")
    }

    Row {
        id: verseActions

        anchors.top: header.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        opacity: 0.0

        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }


        states: [
            State {
                name: "visible"
                when: bibleView.selectedVerses.length > 0

                PropertyChanges {
                    target: verseActions
                    opacity: 1.0
                }
            }
        ]


        EditBubbleButton {
            text: "Copy"
            platformStyle: EditBubbleButtonStyle {
                position: "horizontal-left"
            }

            onClicked: {
                bibleView.copySelectedVerses()
                bibleView.clearSelection()
                copyBanner.show()
            }
        }
        EditBubbleButton {
            text: "Bookmark"
            platformStyle: EditBubbleButtonStyle {
                position: "horizontal-center"
            }
        }
        EditBubbleButton {
            text: "Share"
            platformStyle: EditBubbleButtonStyle {
                position: "horizontal-center"
            }

            onClicked: {
                bibleView.shareSelectedVerses()
                bibleView.clearSelection()
            }
        }
        EditBubbleButton {
            platformStyle: EditBubbleButtonStyle {
                position: "horizontal-right"
            }

            Image {
                anchors.verticalCenter: parent.verticalCenter
                x: 4

                source: 'image://theme/icon-m-toolbar-close'
            }

            onClicked: bibleView.clearSelection()
        }
    }



    Item {
        id: xmlLoadingFailed

        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        visible: (!languages.loading && !languages.loaded) || (settings.translation != null && !settings.translation.loading && !settings.translation.loaded)

        Label {
            id: xmlLoadingFailedLabel

            anchors.verticalCenter: parent.verticalCenter
            anchors.left: parent.left
            anchors.right: parent.right
            anchors.leftMargin: 30
            anchors.rightMargin: 30

            horizontalAlignment: Text.AlignHCenter

            wrapMode: Text.Wrap

            font.pixelSize: 30
            color: '#800'

            text: qsTr("Before you start to read, MeeBible have to load translation list from server")
        }

        MouseArea { anchors.fill: parent }

        Button {
            anchors.top: xmlLoadingFailedLabel.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: xmlLoadingFailedLabel.horizontalCenter

            text: qsTr("Try again")

            onClicked: languages.reload(true)
        }
    }


    Rectangle {
        id: busyIndicator

        anchors.fill: parent

        color: theme.inverted ? '#000' : '#e0e0e0'

        opacity: (languages.loading || (settings.translation && settings.translation.loading) || bibleView.loadingChapter) ? 0.8 : 0.0

        BusyIndicator {
            id: realBusyIndicator

            anchors.centerIn: parent

            running: busyIndicator.visible && busyIndicator.opacity > 0.0

            platformStyle: BusyIndicatorStyle {
                size: "large"
            }
        }

        Label {
            id: busyLabel

            anchors.top: realBusyIndicator.bottom
            anchors.topMargin: 30
            anchors.horizontalCenter: realBusyIndicator.horizontalCenter

            font.pixelSize: 36

            text: qsTr("Loading chapter")
        }

        MouseArea {
            anchors.fill: parent
        }

        Behavior on opacity {
            NumberAnimation {
                duration: 100
            }
        }

        states: [
            State {
                name: "chapter"
                when: bibleView.loadingChapter

                PropertyChanges { target: busyLabel; text: qsTr("Loading chapter") }
            },
            State {
                name: "languages"
                when: languages.loading

                PropertyChanges { target: busyLabel; text: qsTr("Loading translation list") }
            },
            State {
                name: "translation"
                when: settings.translation.loading

                PropertyChanges { target: busyLabel; text: qsTr("Loading book list") }
            }
        ]
    }


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


    QueryDialog {
        id: xmlLoadingFailedNotice

        titleText: qsTr("Cannot load translation list")

        message: qsTr("Please check internet connection")

        acceptButtonText: qsTr("OK")

        Connections {
            target: languages
            onLoadingError: xmlLoadingFailedNotice.open()
        }
        Connections {
            target: settings.translation
            onLoadingError: xmlLoadingFailedNotice.open()
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
            visible: ! NOSEARCH
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


    Loader {
        id: settingsPage

        function load() { source = "SettingsPage.qml" }
        function open() { load(); pageStack.push(item) }
    }

    Loader {
        id: aboutPage

        function load() { source = "AboutPage.qml" }
        function open() { load(); pageStack.push(item) }
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
                onClicked: settingsPage.open()
            }

            MenuItem {
                text: qsTr("About MeeBible")
                onClicked: aboutPage.open()
            }
        }
    }
}
