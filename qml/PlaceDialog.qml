import QtQuick 1.1
import com.meego 1.0


ModelSelectionDialog {
    titleText: "Select Book"


    function bookCode() {
        return model.bookCodeAt(selectedIndex)
    }
}
