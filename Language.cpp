#include "Language.h"

#include <QCoreApplication>
#include <QSqlDatabase>
#include <QSqlQuery>

#include <QDebug>
#include <QVariant>

#include <QListIterator>


QList<Language> Language::_allLanguages;


Language::Language(const QString& code, const QString& engname, const QString& selfname)
    : _code(code), _engname(engname), _selfname(selfname)
{
}



void Language::load()
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
        _allLanguages.append(Language(
            select.value(0).toString(),
            select.value(1).toString(),
            select.value(2).toString()
        ));
}


const Language* Language::langByCode(const QString &code)
{
    for (int i = 0; i < _allLanguages.size(); i++)
        if (_allLanguages[i]._code == code)
            return &_allLanguages[i];

    return 0;
}
