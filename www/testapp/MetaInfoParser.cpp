#include "MetaInfoParser.h"

#include <QDebug>

#include "Languages.h"
#include "Language.h"


MetaInfoParser::MetaInfoParser(Languages* languages)
    : _languages(languages)
{
}


bool MetaInfoParser::startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& attrs)
{
    if (qName == "language")
        _languages->addLanguage(new Language(
            attrs.value("code"),
            attrs.value("name"),
            attrs.value("selfname")
        ));

    return true;
}
