import QtQuick 1.1

// for EditBubbleButton
import "/usr/lib/qt4/imports/com/meego/" 1.0

import com.meego.extras 1.0

import MeeBible 0.1
import "unicomponents"

Page {
    id: page



    property bool created: false
    property bool firstScrollPosSet: false

    Binding {
        target: settings
        property: "place"
        value: bibleView.place
        when: created
    }
    Binding {
        target: settings
        property: "fontSize"
        value: fontSizeSlider.fontSize()
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
        bibleView.place = settings.place
        fontSizeSlider.setFontSize(settings.fontSize)
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
                id: floatingHeader
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
                    if (verseActions.visible)
                        scrollTo(Math.max(y - verseActions.height, 0))
                    else
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
    InfoBanner {
        id: bookmarkBanner
    }

    Row {
        id: verseActions

        anchors.top: header.bottom
        anchors.horizontalCenter: parent.horizontalCenter

        anchors.topMargin: Math.max(floatingHeader.height - flickable.contentY, 0)

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


        property bool narrow: false


        EditBubbleButton {
            text: qsTr("Copy")
            platformStyle: EditBubbleButtonStyle {
                position: "horizontal-left"
                buttonPaddingLeft: verseActions.narrow ? 0 : 8
                buttonPaddingRight: verseActions.narrow ? 0 : 8
            }

            onClicked: {
                bibleView.copySelectedVerses()
                bibleView.clearSelection()
                copyBanner.show()
            }
        }
        EditBubbleButton {
            visible: ! NOSHARE

            text: qsTr("Share")
            platformStyle: EditBubbleButtonStyle {
                position: "horizontal-center"
                buttonPaddingLeft: verseActions.narrow ? 0 : 8
                buttonPaddingRight: verseActions.narrow ? 0 : 8
            }

            onClicked: {
                bibleView.shareSelectedVerses()
                bibleView.clearSelection()
            }
        }
        EditBubbleButton {
            platformStyle: EditBubbleButtonStyle {
                position: "horizontal-center"
                buttonPaddingLeft: verseActions.narrow ? 0 : 8
                buttonPaddingRight: verseActions.narrow ? 0 : 8
            }

            text: qsTr("Bookmark")

            onClicked: {
                if (bibleView.bookmarkSelectedVerses())
                    bookmarkBanner.text = qsTr("Bookmarked")
                else
                    bookmarkBanner.text = qsTr("Already in bookmarks")
                bookmarkBanner.show()

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

        Component.onCompleted: {
            narrow = width > 480
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

        UniLargeBusyIndicator {
            id: realBusyIndicator

            anchors.centerIn: parent

            running: busyIndicator.visible && busyIndicator.opacity > 0.0
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

        function open() { load(); item.open(bibleView.place) }

        Connections {
            target: placeDialog.item
            onAccepted: bibleView.place = placeDialog.item.place()
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
                // bibleView.setAndLoad(bookCode, chapterNo, 1)
                bibleView.place = place
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

        UniToolIcon {
            platformIconId: "toolbar-previous"
            onClicked: bibleView.loadPrevChapter()
        }

        UniToolIcon {
            platformIconId: "toolbar-list"
            onClicked: placeDialog.open()
        }

        UniToolIcon {
            platformIconId: "toolbar-next"
            onClicked: bibleView.loadNextChapter()
        }


        UniToolIcon {
            iconSource: theme.inverted ? "bookmarks-inverted.png" : "bookmarks.png"
            onClicked: bookmarksSheet.open()
        }

        UniToolIcon {
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

            onClicked: fontSizeSlider.value -= 1
        }


        Slider {
            id: fontSizeSlider
            minimumValue: 8  // 16
            maximumValue: 24 // 60
            stepSize: 1.0

            valueIndicatorVisible: true

            anchors.left: fontSizeSmaller.right
            anchors.right: fontSizeBigger.left

            function fontSize() {
                return value * value / 4
            }
            function setFontSize(size) {
                value = parseInt(Math.sqrt(size * 4))
            }

            onValueChanged: {
                var realSize = fontSize()

                var factor = parseFloat(realSize) / bibleView.fontSize
                bibleView.fontSize = realSize
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

            onClicked: fontSizeSlider.value += 1
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

        UniToolIcon {
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

        UniToolIcon {
            platformIconId: "toolbar-down"

            onClicked: {
                if (bibleView.matchIndex < bibleView.matchCount - 1)
                    bibleView.matchIndex = bibleView.matchIndex + 1
                else
                    bibleView.matchIndex = 0
            }
        }

        UniToolIcon {
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

    Loader {
        id: bookmarksSheet
        function load() { source = "BookmarksSheet.qml" }
        function open() { load(); item.open() }

        Connections {
            target: bookmarksSheet.item
            onBookmarkSelected: bibleView.place = place
        }
    }



    Menu {
        id: menu

        visualParent: pageStack

        MenuLayout {
            MenuItem {
                text: qsTr("Search")

                onClicked: searchDialog.open()
                visible: ! NOSEARCH
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
