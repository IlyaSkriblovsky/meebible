import QtQuick 1.1
import com.meego 1.0
import MeeBible 0.1


ModelSelectionDialog {
    width: parent.width - 60

    titleText: "Select Language"

    model: languages


    function language() {
        return languages.langAt(selectedIndex)
    }
}
