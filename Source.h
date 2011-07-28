#ifndef SOURCE_H
#define SOURCE_H

#include <QObject>


class TranslationsList;
class Languages;


class Source: public QObject
{
    Q_OBJECT

public:
    virtual void addTranslationsToList(Languages* languages, TranslationsList *list) = 0;
};

#endif // SOURCE_H
