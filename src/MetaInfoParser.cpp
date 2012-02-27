#include "MetaInfoParser.h"

#include <QDebug>

#include "Languages.h"
#include "Language.h"
#include "Translation.h"


MetaInfoParser::MetaInfoParser(Languages* languages)
    : _languages(languages)
{
}


bool MetaInfoParser::startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& attrs)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    if (qName == "language")
        _languages->addLanguage(new Language(
            attrs.value("code"),
            attrs.value("name"),
            attrs.value("selfname")
        ));

    if (qName == "trans")
    {
        _transCode = attrs.value("code");
        _transSourceUrl = attrs.value("sourceUrl");
        _transCopyright = attrs.value("copyright");
        _transRtl = attrs.value("rtl") == "1";
    }

    if (qName == "transLang")
    {
        Language* lang = _languages->langByCode(attrs.value("code"));
        lang->addTranslation(new Translation(
            _transCode,
            lang,
            attrs.value("name"),
            _transSourceUrl,
            _transCopyright,
            _transRtl
        ));
    }

    return true;
}
