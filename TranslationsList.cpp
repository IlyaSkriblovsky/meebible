#include "TranslationsList.h"

TranslationsList::TranslationsList()
{
}


void TranslationsList::addTranslation(const Language *lang, Translation *translation)
{
    _translations.append(QPair<const Language*, Translation*>(lang, translation));
}

QList<Translation*> TranslationsList::translationsForLang(const Language *lang)
{
    QList<Translation*> result;

    for (int i = 0; i < _translations.size(); ++i)
        if (_translations[i].first == lang)
            result.append(_translations[i].second);

    return result;
}
