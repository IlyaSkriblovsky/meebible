import QtQuick 1.1
import com.meego 1.0

ModelSelectionDialog {
    titleText: "Select Translation"

    listHeight: 150


    function translation() {
        return model.translationAt(selectedIndex)
    }
}
