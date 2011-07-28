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


QList<Language*> Languages::all()
{
    return _languages;
}


QVariantList Languages::all_js()
{
    return Utils::objectListToVariantList(_languages);
}
