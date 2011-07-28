#ifndef LANGUAGES_H
#define LANGUAGES_H

#include <QList>

#include <QVariantList>

#include "Language.h"

class Languages: public QObject
{
    Q_OBJECT

public:
    Languages();
    ~Languages();

    Q_INVOKABLE Language* langByCode(const QString& code);

    QList<Language*> all();
    Q_INVOKABLE QVariantList all_js();

private:
    QList<Language*> _languages;
};

#endif // LANGUAGES
