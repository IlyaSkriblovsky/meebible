import QtQuick 1.1
import com.meego 1.0

Page {

    Header {
        id: header
        text: "About MeeBible"
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
                text: "Thank you for using MeeBible, the open source Bible reader for Harmattan"

                width: parent.width
            }

            Label {
                text: "Currently following Bible translations are supported: <ul> <li>New World Translationi in 40 languages</li> <li>Russian Synodal translation</li> </ul>"

                width: parent.width
            }

            Label {
                text: "Read God's Word daily!"
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
                text: '© 2011 Ilya Skriblovsky<br>E-Mail: <a href="mailto:IlyaSkriblovsky@gmail.com">Ilya.Skriblovsky@gmail.com</a>'
                width: parent.width

                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {
                text: 'Please contact me with any comments or suggestions or if you want to add new Bible or UI translations.'

                width: parent.width
            }

            Label {
                text: 'Homepage & sources: <a href="http://projects.developer.nokia.com/meebible">http://projects.developer.nokia.com/meebible</a>'
                width: parent.width

                onLinkActivated: Qt.openUrlExternally(link)
            }

            Label {
                text: "New World Translation © Watch Tower Bible and Tract Society of Pennsylvania"
                width: parent.width
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
