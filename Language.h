#ifndef LANGUAGE_H
#define LANGUAGE_H

#include <QList>
#include <QString>


class Language
{
public:
    Language(const QString& code, const QString& engname, const QString& selfname);

    QString code() const { return _code; }
    QString engname() const { return _engname; }
    QString selfname() const { return _selfname; }

    static void load();

    static const Language* langByCode(const QString& code);
    static QList<const Language*> allLanguages();

private:
    static QList<Language> _allLanguages;

    QString _code;
    QString _engname;
    QString _selfname;
};

#endif // LANGUAGE_H
