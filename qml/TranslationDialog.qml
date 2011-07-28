import QtQuick 1.1
import com.meego 1.0

SelectionDialog {
    titleText: "Select Translation"

    property string languageCode

    model: ListModel {
        id: transModel
    }

    onLanguageCodeChanged: {
        transModel.clear()

        var lang = languages.langByCode(languageCode)
        var trans = translations.translationsForLang_js(lang)
        for (var i = 0; i < trans.length; i++)
            transModel.append({name: trans[i].name})
    }

    function translation()
    {
        var lang = languages.langByCode(languageCode)
        var trans = translations.translationsForLang_js(lang)

        console.log( trans[selectedIndex])
        return trans[selectedIndex]
    }
}
