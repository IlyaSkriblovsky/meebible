#ifndef PARSERFACTORY_H
#define PARSERFACTORY_H

#include <QCache>

class Parser;

class ParserFactory
{
    public:
        static Parser* getParser(const QString& parserName);

    protected:
        static QCache<QString, Parser> _cache;

        static Parser* create(const QString& parserName);
};

#endif
