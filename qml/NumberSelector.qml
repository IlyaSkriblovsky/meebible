import QtQuick 1.1
import com.meego 1.0


Column {
    property alias title: label.text
    property alias minimum: slider.minimumValue
    property alias maximum: slider.maximumValue
    property alias value: field.text

    Row {
        anchors.left: parent.left
        anchors.right: parent.right
        anchors.leftMargin: 20

        Label {
            id: label

            font.pixelSize: 32
            
            anchors.verticalCenter: field.verticalCenter
        }

        TextField {
            id: field

            text: slider.minimumValue

            inputMethodHints: Qt.ImhDigitsOnly

            validator: IntValidator {
                bottom: slider.minimumValue
                top: slider.maximumValue
            }

        }
    }

    Slider {
        id: slider

        anchors.left: parent.left
        anchors.right: parent.right

        value: field.text

        stepSize: 1

        onValueChanged: {
            field.text = value
        }
    }
}
