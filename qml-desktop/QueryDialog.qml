import QtQuick 2.15
import QtQuick.Controls 2.15

Dialog {
    id: inputDialog

    property alias titleText: inputDialog.title
    property alias message: label.text

    standardButtons: Dialog.Ok | Dialog.Cancel
    modal: true

    contentItem: Label {
        id: label
        wrapMode: Text.Wrap
        padding: 16
    }
}