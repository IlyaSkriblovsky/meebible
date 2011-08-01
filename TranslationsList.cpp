#include "TranslationsList.h"

#include <QDebug>
#include <QHash>
#include <QByteArray>

#include "Translation.h"
#include "Utils.h"
#include "Language.h"


TranslationsList::TranslationsList()
{
    QHash<int, QByteArray> roleNames;
    roleNames[LanguageRole] = "language";
    roleNames[TranslationRole] = "translation";
    roleNames[LanguageCodeRole] = "languageCode";
    roleNames[TranslationNameRole] = "translationName";
}


TranslationsList::~TranslationsList()
{
    for (int i = 0; i < _translations.size(); ++i)
        delete _translations[i].second;
}


void TranslationsList::addTranslation(Language *lang, Translation *translation)
{
    _translations.append(QPair<Language*, Translation*>(lang, translation));
}

QList<Translation*> TranslationsList::translationsForLang(const Language *lang)
{
    QList<Translation*> result;


    for (int i = 0; i < _translations.size(); ++i)
    {
        if (_translations[i].first->code() == lang->code())
            result.append(_translations[i].second);
    }

    return result;
}



int TranslationsList::rowCount(const QModelIndex& index) const
{
    Q_UNUSED(index)

    return _translations.size();
}

QVariant TranslationsList::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0  ||  index.row() >= _translations.size())
        return QVariant();

    switch (role)
    {
        case LanguageRole: return QVariant::fromValue((QObject*)_translations.at(index.row()).first);
        case LanguageCodeRole: return _translations.at(index.row()).first->code();

        case TranslationRole: return QVariant::fromValue((QObject*)_translations.at(index.row()).second);

        case Qt::DisplayRole:
        case TranslationNameRole: return _translations.at(index.row()).second->name();
    }

    return QVariant();
}
