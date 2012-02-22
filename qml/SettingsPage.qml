import QtQuick 1.1
import com.meego 1.0
import com.meego.extras 1.0

Page {
    id: settingsPage


    property bool __created: false


    TumblerButtonStyle {
        id: tumblerStyleBlue

        textColor: theme.inverted ? '#aaf' : '#006'
    }


    Header {
        id: header
        text: qsTr("Settings")
    }



    Flickable {
        id: flickable

        clip: true

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.bottom: parent.bottom

        flickableDirection: Flickable.VerticalFlick

        contentWidth: column.width
        contentHeight: column.height

        Column {
            id: column

            width: settingsPage.width

            TumblerItem {
                titleText: qsTr("Language")
                valueText: settings.language ? settings.language.selfname : qsTr('<not loaded yet>')

                onClicked: languageDialog.open()
            }

            TumblerItem {
                titleText: qsTr("Translation")
                valueText: settings.translation ? settings.translation.name : qsTr('<not loaded yet>')

                onClicked: transDialog.open()
            }

            Rectangle {
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10

                height: childrenRect.height + 20

                radius: 5

                color: 'transparent'

                border.color: '#888'
                border.width: 1

                Column {
                    anchors.left: parent.left
                    anchors.right: parent.right
                    anchors.margins: 20

                    y: 10

                    Label {
                        anchors.left: parent.left
                        anchors.right: parent.right

                        text: settings.translation ? 'Available at <a href="' + settings.translation.sourceUrl + '">' + settings.translation.sourceUrl + '</a>' : ''
                        font.pixelSize: 20

                        onLinkActivated: Qt.openUrlExternally(settings.translation.sourceUrl)
                    }
                    Label {
                        anchors.left: parent.left
                        anchors.right: parent.right

                        text: settings.translation ? settings.translation.copyright : ''
                        visible: settings.translation != null  &&  settings.translation.copyright != ''
                        font.pixelSize: 20
                    }
                }
            }

            Item {
                width: 1
                height: 20
            }

            TumblerItem {
                titleText: qsTr("Font")
                valueText: settings.fontName

                onClicked: fontDialog.open()
            }

            LabeledSwitch {
                text: qsTr("Scroll header with the text")

                checked: settings.floatingHeader
                onCheckedChanged: settings.floatingHeader = checked
            }

            LabeledSwitch {
                text: qsTr("Fullscreen")

                checked: ! window.showStatusBar
                onCheckedChanged: window.showStatusBar = ! checked
            }

            LabeledSwitch {
                text: qsTr("Inverted theme")

                checked: theme.inverted
                onCheckedChanged: theme.inverted = checked
            }


            ButtonWithHelp {
                buttonText: qsTr("Update translation list")
                helpTitle: qsTr("Reload languages")
                helpMessage: qsTr("When new translation is published on MeeBible's server, use this button to update translation list")

                onClicked: languages.reload(false)
            }


            ButtonWithHelp {
                buttonText: qsTr("Clear cache")
                helpTitle: qsTr("Cache clearing")
                helpMessage: qsTr("This will delete all downloaded Bible chapters from your phone. Do this to force reloading chapters when some error was fixed in translation on the server side.")

                onClicked: clearCacheConfirmation.open()
            }
        }
    }
    ScrollDecorator {flickableItem: flickable }


    Loader {
        id: languageDialog

        width: 10
        height: 10

        function load() { source = "LanguageDialog.qml" }

        function open() { load(); item.open() }

        Connections {
            target: languageDialog.item
            onAccepted: {
                settings.language = languageDialog.item.language()

                transDialog.open()
            }
        }
    }

    Loader {
        id: transDialog

        width: 10; height: 10

        function load() { source = "TranslationDialog.qml" }

        function open() { load(); item.open() }

        Connections {
            target: transDialog.item
            onAccepted: settings.translation = transDialog.item.translation()
        }
    }


    ModelSelectionDialog {
        id: fontDialog

        titleText: qsTr("Select Font")

        header: Label {
            text: qsTr("Not all fonts can display all languages")

            color: "#666"
            font.family: "Nokia Pure Text Light"

            wrapMode: Text.Wrap

            anchors.left: parent.left
            anchors.right: parent.right
        }

        model: ListModel {
            id: fontModel

            ListElement { value: "Nokia Pure Text" }
            ListElement { value: "Nokia Pure Text Light" }
            ListElement { value: "Nokia Standard" }
            ListElement { value: "DejaVu Sans" }
            ListElement { value: "Times" }
            ListElement { value: "Georgia" }
            ListElement { value: "Trebuchet" }
        }

        onAccepted: {
            settings.fontName = fontModel.get(selectedIndex).value
        }
    }


    QueryDialog {
        id: clearCacheConfirmation

        titleText: qsTr("Do you want to clear the cache?")

        message: qsTr("This will delete all downloaded Bible chapters from your phone")

        acceptButtonText: qsTr("Clear")
        rejectButtonText: qsTr("Cancel")

        onAccepted: {
            cache.clearCache()
            settings.translation.reload(false)
            bibleView.loadChapter()
        }
    }


    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }
}
