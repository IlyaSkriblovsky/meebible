import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15


ModelSelectionDialog {
    // width: parent.width - 60

    titleText: qsTr("Select Language")

    model: languages


    function language() {
        return languages.langAt(selectedIndex)
    }
}
