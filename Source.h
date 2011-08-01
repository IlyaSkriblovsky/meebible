#ifndef SOURCE_H
#define SOURCE_H

#include <QObject>


class Languages;


class Source: public QObject
{
    Q_OBJECT

public:
    virtual void addTranslationsToList(Languages* languages) = 0;
};

#endif // SOURCE_H
