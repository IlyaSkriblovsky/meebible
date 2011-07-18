#ifndef SOURCE_H
#define SOURCE_H

#include <QObject>


class TranslationsList;


class Source: public QObject
{
    Q_OBJECT

public:
    virtual void addTranslationsToList(TranslationsList *list) = 0;
};

#endif // SOURCE_H
