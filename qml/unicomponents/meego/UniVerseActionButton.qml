import Qt 4.7

// for EditBubbleButton
import "/usr/lib/qt4/imports/com/nokia/meego/" 1.1

EditBubbleButton {
    id: button

    property alias iconSource: icon.source

    property string buttonType: "c"
    property bool narrow: false

    property int innerWidth: width

    platformStyle: EditBubbleButtonStyle {
        id: buttonStyle

        position: {
            switch (button.buttonType)
            {
                case "l": return "horizontal-left"
                case "r": return "horizontal-right"
                default : return "horizontal-center"
            }
        }

        buttonPaddingLeft : (icon.source != "" ? 10 : 0) + (button.narrow ? 0 : 8)
        buttonPaddingRight: (icon.source != "" ? 10 : 0) + (button.narrow ? 0 : 8)
    }


    Image {
        id: icon

        anchors.verticalCenter: parent.verticalCenter
        // anchors.horizontalCenter: parent.horizontalCenter
        x: (button.width - icon.width) / 2 - 4

        visible: source != ""
    }
}
