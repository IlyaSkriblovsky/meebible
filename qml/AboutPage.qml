import QtQuick 1.1
import com.meego 1.0

Page {

    Header {
        id: header
        text: qsTr("About MeeBible")
    }

    Flickable {
        id: flickable

        anchors.top: header.bottom
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.bottom: parent.bottom

        clip: true

        contentWidth: width
        contentHeight: column.height + 20

        Column {
            id: column

            x: 10
            y: 10
            width: parent.width - 20

            spacing: 20


            Label {
                text: qsTr("MeeBible — the open source Bible reader for Harmattan")

                width: parent.width
            }

            Label {
                text: freeversion ? qsTr("If you want to support development of MeeBible, please consider buying <a href='http://store.ovi.com/content/215454'>paid version from Ovi Store</a>!") : qsTr("Thank you for supporting MeeBible!")

                width: parent.width

                color: freeversion ? "#800000" : "#008000"

                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {
                text: qsTr("Read God's Word daily!")
            }

            // Rectangle {
            //     height: 2
            //     width: parent.width
            //     color: '#fff'

            //     Rectangle {
            //         height: 1
            //         width: parent.width
            //         color: '#d6cfd6'
            //     }
            // }

            Button {
                anchors.horizontalCenter: parent.horizontalCenter

                text: qsTr("Send your feedback")

                height: 60

                Image {
                    source: "image://theme/icon-s-common-next"
                    anchors.right: parent.right
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.rightMargin: 10
                }

                Loader {
                    id: feedbackPage

                    function load() { source = "FeedbackPage.qml" }

                    function open() { load(); pageStack.push(item) }
                }

                onClicked: feedbackPage.open()
            }

            Label {
                text: qsTr('© 2011 Ilya Skriblovsky<br>E-Mail: <a href="mailto:Ilya.Skriblovsky@gmail.com?subject=MeeBible">Ilya.Skriblovsky@gmail.com</a>')
                width: parent.width

                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {
                text: qsTr("Please contact me with any comments or suggestions or if you want to add new Bible or UI translations.")

                width: parent.width
            }

            Label {
                text: qsTr('Homepage & sources: <a href="http://meebible.org">http://meebible.org</a>')
                width: parent.width

                onLinkActivated: Qt.openUrlExternally(link)
            }
        }
    }
    ScrollDecorator { flickableItem: flickable }

    tools: ToolBarLayout {
        ToolIcon {
            platformIconId: "toolbar-back"
            onClicked: pageStack.pop()
        }
    }
}
