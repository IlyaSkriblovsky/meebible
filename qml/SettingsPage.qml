import QtQuick 1.1
import com.nokia.meego 1.0
import com.meego.extras 1.0

Page {

    TumblerButtonStyle {
        id: tumblerStyleBlue

        textColor: '#006'
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

        contentWidth: width
        contentHeight: column.height + 20

        Column {
            id: column

            x: 10
            y: 10
            width: parent.width - 20

            spacing: 20

            Label {
                text: "Language:"
                font.bold: true
            }

            TumblerButton {
                text: settings.language.selfname

                x: 40
                width: parent.width - 80

                style: tumblerStyleBlue

                onClicked: languageDialog.open()
            }

            Label {
                text: "Translation:"
                font.bold: true
            }

            TumblerButton {
                text: settings.translation.name

                x: 40
                width: parent.width - 80

                style: tumblerStyleBlue

                onClicked: transDialog.open()
            }

            Rectangle {
                width: 1
                height: 10
            }

            Label {
                text: "Floating header in reading mode"
                width: parent.width
                font.bold: true

                Switch {
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter

                    checked: settings.floatingHeader

                    onCheckedChanged: settings.floatingHeader = checked
                }
            }

            Rectangle {
                height: 2
                width: parent.width
                color: '#fff'

                Rectangle {
                    height: 1
                    width: parent.width
                    color: '#d6cfd6'
                }
            }

            Label {
                text: "Bible chapters are usually downloaded when you open them for the first time. But you can also download full Bible text at once. This may take 10–15 minutes and you'd better to do it over WiFi network."
                width: parent.width
            }

            Label {
                text: "<b>Note:</b> you have to download full text if you want to use search feature"
                width: parent.width
            }

            Button {
                text: "Download Bible"
                x: 50
                width: parent.width - 100

                onClicked: fetcherDialog.start()
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

    Loader {
        id: fetcherDialog

        width: 10; height: 10

        function load() { source = "FetcherDialog.qml" }
        function open() { load(); item.open() }

        function start() { load(); item.start() }
    }


    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }
}
