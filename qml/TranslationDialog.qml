import QtQuick 1.1
import com.meego 1.0

ModelSelectionDialog {
    titleText: qsTr("Select Translation")

    listHeight: 250

    model: settings.language

    function translation() {
        return model.translationAt(selectedIndex)
    }
}
