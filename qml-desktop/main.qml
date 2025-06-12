import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls

ApplicationWindow {
    width: 480
    height: 640
    visible: true
    title: "MeeBible"

    StackView {
        id: stackView
        anchors.fill: parent

        initialItem: BibleViewPage {
            stack: stackView
        }
    }
}
