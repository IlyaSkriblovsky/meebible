#include "ParserFactory.h"

#include "NWTParser.h"
#include "BibleOnlineParser.h"
#include "BibeleLVParser.h"
#include "CCArabicParser.h"
#include "KJBOParser.h"


QCache<QString, Parser> ParserFactory::_cache;


Parser* ParserFactory::getParser(const QString& parserName)
{
    if (_cache.contains(parserName))
        return _cache.object(parserName);

    Parser* parser = create(parserName);
    _cache.insert(parserName, parser);

    return parser;
}

Parser* ParserFactory::create(const QString& parserName)
{
    if (parserName == "NWTParser")
        return new NWTParser;

    if (parserName == "BibleOnlineParser")
        return new BibleOnlineParser;

    if (parserName == "BibeleLVParser")
        return new BibeleLVParser;

    if (parserName == "CCArabicParser")
        return new CCArabicParser;

    if (parserName == "KJBOParser")
        return new KJBOParser;

    return 0;
}
