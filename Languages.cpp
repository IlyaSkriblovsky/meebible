#include "Languages.h"

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <QDebug>
#include <QVariant>

#include "Utils.h"


Languages::Languages()
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", "langs");
    db.setDatabaseName("../MeeBible2/langs.sqlite");
    if (! db.open())
    {
        qDebug() << "Cannot open langs db";
        QCoreApplication::exit(1);
    }


    QSqlQuery select("SELECT * FROM langs ORDER BY engname", db);
    while (select.next())
        _languages.append(new Language(
            select.value(0).toString(),
            select.value(1).toString(),
            select.value(2).toString()
        ));


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
    Language* z = _languages.at(row);
    qDebug() << "z=" << z;
    return z;
}
