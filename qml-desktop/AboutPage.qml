import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

Page {

    id: aboutPage

    header: ToolBar {
        RowLayout {
            anchors.fill: parent
            spacing: 10

            ToolButton {
                text: "< Back"
                onClicked: { aboutPage.StackView.view?.pop() }
            }

            Label {
                text: qsTr("About")
                font.bold: true
                Layout.fillWidth: true
                horizontalAlignment: Text.AlignHCenter
            }
        }
    }


    ScrollView {
        anchors.fill: parent

        Column {
            spacing: 16
            x: 30
            width: aboutPage.width - 60
            topPadding: 30
            bottomPadding: 30

            Label {
                text: qsTr("The open source Bible reader")

                font.pixelSize: 26

                horizontalAlignment: Text.AlignHCenter

                wrapMode: Text.Wrap

                width: parent.width
            }


            Label {
                text: '<html><a href="http://meebible.org">MeeBible.org</a></html>'
                textFormat: Text.RichText
                horizontalAlignment: Text.AlignHCenter

                font.pixelSize: 34

                width: parent.width
                wrapMode: Text.Wrap

                onLinkActivated: Qt.openUrlExternally(link)
            }

            Button {
                id: feedbackButton
                anchors.horizontalCenter: parent.horizontalCenter

                text: qsTr("Send your feedback")

                Loader {
                    id: feedbackPage

                    function load() { source = "FeedbackPage.qml" }

                    function open() { load(); pageStack.push(item) }
                }

                onClicked: feedbackPage.open()
            }

//             Button {
//                 visible: IAPDONATION
//
//                 anchors.horizontalCenter: parent.horizontalCenter
//
//                 text: qsTr("Donate")
//
//                 width: feedbackButton.width
//                 height: 60
//
//                 Loader {
//                     id: donatePage
//
//                     function load() { source = "DonatePage.qml" }
//
//                     function open() { load(); pageStack.push(item) }
//                 }
//
//                 onClicked: donatePage.open()
//
//                 $$QML_SYMBIAN_THEME
//             }

            Label {
                text: qsTr("Read God's Word daily!")
                horizontalAlignment: Text.AlignHCenter

                width: parent.width
                wrapMode: Text.Wrap
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

            // Label {
            //     text: "<html>" +
            //         (freeversion ? qsTr("If you want to support development, please consider buying <a href='http://store.ovi.com/content/215454'>paid version</a>!") : qsTr("Thank you for supporting development!")) +
            //         "</html>"
            //     textFormat: Text.RichText
            //
            //     visible: ! SYMBIAN
            //
            //     width: parent.width
            //
            //     color: freeversion ? "#800000" : "#008000"
            //
            //     wrapMode: Text.Wrap
            //
            //     onLinkActivated: Qt.openUrlExternally(link)
            // }

            Label {
                text: '<html>' +
                    qsTr('© 2011—2025 Ilya Skriblovsky') + '<br>E-Mail: <a href="mailto:ilyaskriblovsky@gmail.com?subject=MeeBible">ilyaskriblovsky@gmail.com</a></html>'
                textFormat: Text.RichText
                width: parent.width

                onLinkActivated: Qt.openUrlExternally(link)

                wrapMode: Text.Wrap
            }

            Label {
                visible: SYMBIAN

                text: "<html>" +
                    qsTr("Beta testing: Simon Botes of {AppCRAFT}").replace('{AppCRAFT}', '<a href="http://store.ovi.com/publisher/AppCRAFT">AppCRAFT</a></html>')
                textFormat: Text.RichText
                width: parent.width

                onLinkActivated: Qt.openUrlExternally(link)

                wrapMode: Text.Wrap
            }

            Label {
                text: qsTr("Please contact me with any comments or suggestions or if you want to add new Bible or UI translations.")

                width: parent.width

                wrapMode: Text.Wrap
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
                text: (
                    'Thanks for translating UI to:'
                    + '<ul style="text-indent: 0">'
                    + '<li>' + qsTr("Finnish") + ': Dan Westerlund</li>'
                    + '<li>' + qsTr("German") + ': Martin Augustin</li>'
                    + '<li>' + qsTr("Italian") + ': Paolo Gherpelli</li>'
                    + '<li>' + qsTr("Polish") + ': Mateusz Kitka</li>'
                    + '<li>' + qsTr("Portuguese") + ': Paulo Cecilio Jr</li>'
                    + '<li>' + qsTr("Russian") + ': Ilya Skriblovsky</li>'
                    + '<li>' + qsTr("Spanish") + ': Rafael Rivas</li>'
                    + '<li>' + qsTr("Swedish") + ': David Sikström (True Media, <a href="http://www.truemedia.se">www.truemedia.se</a>) and Dan Westerlund</li>'
                    + '</ul>'
                )
                textFormat: Text.RichText

                width: parent.width
                Layout.preferredWidth: width
                wrapMode: Text.Wrap
            }
        }
    }
}
