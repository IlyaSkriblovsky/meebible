import Qt 4.7

// for EditBubbleButton
import "/usr/lib/qt4/imports/com/nokia/meego/" 1.1

EditBubbleButton {
    id: button

    property string buttonType: "c"
    property bool narrow: false

    platformStyle: EditBubbleButtonStyle {
        position: {
            switch (button.buttonType)
            {
                case "l": return "horizontal-left"
                case "r": return "horizontal-right"
                default : return "horizontal-center"
            }
        }

        buttonPaddingLeft : button.narrow ? 0 : 8
        buttonPaddingRight: button.narrow ? 0 : 8
    }
}
