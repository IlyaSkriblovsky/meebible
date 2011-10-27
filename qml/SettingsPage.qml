import QtQuick 1.1
import com.nokia.meego 1.0
import com.nokia.extras 1.1

Page {
    id: settingsPage

    TumblerButtonStyle {
        id: tumblerStyleBlue

        textColor: theme.inverted ? '#aaf' : '#006'
    }


    Header {
        id: header
        text: "Settings"
    }



    Flickable {
        id: flickable

        clip: true

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.top: header.bottom
        anchors.bottom: parent.bottom

        flickableDirection: Flickable.VerticalFlick

        contentWidth: column.width + 20
        contentHeight: column.height + 40

        Column {
            id: column

            x: 10
            width: settingsPage.width - 20

            y: 20

            spacing: 40

            Column {
                id: language

                width: parent.width

                spacing: 20

                Label {
                    text: "Language"
                    font.bold: true
                }

                TumblerButton {
                    text: settings.language.selfname

                    x: 40
                    width: parent.width - 80

                    style: tumblerStyleBlue

                    onClicked: languageDialog.open()
                }
            }

            Column {
                id: translation

                width: parent.width

                spacing: 20

                Label {
                    text: "Translation"
                    font.bold: true
                }

                TumblerButton {
                    text: settings.translation.name

                    x: 40
                    width: parent.width - 80

                    style: tumblerStyleBlue

                    onClicked: transDialog.open()
                }
            }

            LabeledSwitch {
                text: "Scroll header with the text"

                checked: settings.floatingHeader
                onCheckedChanged: settings.floatingHeader = checked
            }

            LabeledSwitch {
                text: "Fullscreen"

                checked: ! window.showStatusBar
                onCheckedChanged: window.showStatusBar = ! checked
            }

            LabeledSwitch {
                text: "Inverted theme"

                checked: theme.inverted
                onCheckedChanged: theme.inverted = checked
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


    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }
}
