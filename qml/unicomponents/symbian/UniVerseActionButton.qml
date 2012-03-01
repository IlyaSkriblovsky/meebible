import Qt 4.7
import com.nokia.symbian 1.1

Item {
    id: button

    property alias text: label.text

    property string buttonType: "c"
    property bool narrow: false

    signal clicked


    width: content.width
    height: content.height + 10

    Item {
        id: content

        width: label.width + 2*platformStyle.paddingLarge
        height: privateStyle.buttonSize

        anchors.bottom: parent.bottom


        BorderImage {
            source: privateStyle.imagePath("qtg_fr_pushbutton_segmented_" + button.buttonType + "_" + (mouseArea.pressed ? "pressed" : "normal"))
            border { left: 20; top: 20; right: 20; bottom: 20 }
            anchors.fill: parent
        }

        Label {
            id: label

            anchors.centerIn: parent

            font { family: platformStyle.fontFamilyRegular; pixelSize: platformStyle.fontSizeLarge }
        }

        MouseArea {
            id: mouseArea
            anchors.fill: parent

            onClicked: button.clicked()
        }
    }
}
