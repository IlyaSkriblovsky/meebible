import QtQuick 1.1
import com.meego 1.0
import com.meego.extras 1.0

Page {
    id: settingsPage


    property bool __created: false

    Binding {
        target: settings
        property: 'inverted'
        value: theme.inverted
        when: __created
    }
    Component.onCompleted: {
        theme.inverted = settings.inverted
        __created = true
    }


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

        contentWidth: column.width + 20
        contentHeight: column.height + 40

        Column {
            id: column

            x: 10
            width: settingsPage.width - 20

            y: 20

            spacing: 30

            Column {
                id: language

                width: parent.width

                spacing: 20

                Label {
                    text: qsTr("Language")
                    font.bold: true
                }

                TumblerButton {
                    text: settings.language ? settings.language.selfname : "<not loaded yet>"

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
                    text: qsTr("Translation")
                    font.bold: true
                }

                TumblerButton {
                    text: settings.translation ? settings.translation.name : "<not loaded yet>"

                    x: 40
                    width: parent.width - 80

                    style: tumblerStyleBlue

                    onClicked: transDialog.open()
                }

                Label {
                    text: settings.translation ? 'Available at <a href="' + settings.translation.sourceUrl + '">' + settings.translation.sourceUrl + '</a>' : ''
                    font.pixelSize: 20
                    x: 40
                    width: parent.width - x

                    onLinkActivated: Qt.openUrlExternally(settings.translation.sourceUrl)
                }
                Label {
                    text: settings.translation ? settings.translation.copyright : ''
                    font.pixelSize: 20
                    x: 40
                    width: parent.width - x
                }
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

            Button {
                text: qsTr("Clear cache")

                anchors.horizontalCenter: parent.horizontalCenter

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


    QueryDialog {
        id: clearCacheConfirmation

        titleText: qsTr("Do you want to clear the cache?")

        message: qsTr("This will delete all downloaded Bible chapters from your phone")

        acceptButtonText: qsTr("Clear")
        rejectButtonText: qsTr("Cancel")

        onAccepted: cache.clearCache()
    }


    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }
}
