import QtQuick 1.1
import com.meego 1.0
import MeeBible 0.1


SelectionDialog {
    property string selectedLanguageCode

    titleText: "Select Language"

    model: ListModel {
        id: languagesModel
    }

    Component.onCompleted: {
        var langs = languages.all_js()
        for (var i = 0; i < langs.length; i++)
        {
            var lang = langs[i]
            languagesModel.append({name: lang.engname + ' (' + lang.selfname + ')'})
        }
    }

    onAccepted: selectedLanguageCode = languages.all_js()[selectedIndex].code
}
