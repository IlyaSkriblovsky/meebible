#include "Languages.h"

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <QDebug>
#include <QVariant>

#include "Paths.h"
#include "MetaInfoLoader.h"
#include "MetaInfoParser.h"


Languages::Languages()
    : _loading(false)
{
    QHash<int, QByteArray> roleNames;
    roleNames[Qt::DisplayRole] = "value";
    roleNames[CodeRole] = "code";
    roleNames[EngnameRole] = "engname";
    roleNames[SelfnameRole] = "selfname";
    setRoleNames(roleNames);
}

Languages::~Languages()
{
    for (int i = 0; i < _languages.size(); i++)
        delete _languages.at(i);
}



void Languages::addLanguage(Language* language)
{
    beginInsertRows(QModelIndex(), _languages.size(), _languages.size());
    _languages.append(language);
    endInsertRows();
}



Language* Languages::langByCode(const QString& code)
{
    for (int i = 0; i < _languages.size(); i++)
        if (_languages[i]->code() == code)
            return _languages[i];


    return 0;
}


int Languages::rowCount(const QModelIndex& index) const
{
    Q_UNUSED(index)

    return _languages.size();
}

QVariant Languages::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= _languages.size())
        return QVariant();

    switch (role)
    {
        case Qt::DisplayRole:
        {
            Language* lang = _languages.at(index.row());
            return lang->engname() + " (" + lang->selfname() + ")";
        }

        case SelfnameRole:
            return _languages.at(index.row())->selfname();

        case CodeRole:
            return _languages.at(index.row())->code();

        case EngnameRole:
            return _languages.at(index.row())->engname();
    }

    return QVariant();
}


Language* Languages::langAt(int row) const
{
    return _languages.at(row);
}




void Languages::reload()
{
    if (MetaInfoLoader::instance()->loadLangsAndTransInfo(this))
        setLoading(true);
}

void Languages::loadFromXML(const QString& xml)
{
    clear();

    MetaInfoParser handler(this);
    QXmlSimpleReader reader;
    reader.setContentHandler(&handler);
    reader.setErrorHandler(&handler);

    QXmlInputSource source;
    source.setData(xml);

    reader.parse(source);

    loadingFinished();
    setLoading(false);
    loadedChanged();
}

void Languages::setLoading(bool loading)
{
    if (_loading != loading)
    {
        _loading = loading;
        loadingChanged();
    }
}

void Languages::clear()
{
    beginResetModel();

    for (int i = 0; i < _languages.size(); i++)
        delete _languages.at(i);

    _languages.clear();

    endResetModel();
    loadedChanged();
}
