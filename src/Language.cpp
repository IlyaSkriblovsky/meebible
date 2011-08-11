#include "Language.h"

#include <QHash>
#include <QByteArray>

#include "Translation.h"


Language::Language(const QString& code, const QString& engname, const QString& selfname)
    : _code(code), _engname(engname), _selfname(selfname)
{
    QHash<int, QByteArray> roleNames;
    roleNames[Qt::DisplayRole] = "value";
    roleNames[TranslationRole] = "translation";
    roleNames[TranslationNameRole] = "translationName";
    setRoleNames(roleNames);
}


void Language::addTranslation(Translation* translation)
{
    translation->setParent(this);
    _translations.append(translation);
}


int Language::rowCount(const QModelIndex& index) const
{
    Q_UNUSED(index)

    return _translations.size();
}


QVariant Language::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0  ||  index.row() >= _translations.size())
        return QVariant();

    switch (role)
    {
        case TranslationRole: return QVariant::fromValue((QObject*)_translations.at(index.row()));

        case Qt::DisplayRole:
        case TranslationNameRole:
            return _translations.at(index.row())->name();
    }

    return QVariant();
}


Translation* Language::translationAt(int row) const
{
    return _translations.at(row);
}


Translation* Language::translationByCode(const QString& code) const
{
    for (int i = 0; i < _translations.size(); i++)
        if (_translations[i]->code() == code)
            return _translations[i];

    return 0;
}
