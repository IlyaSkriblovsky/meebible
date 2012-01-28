#include "TranslationInfoParser.h"

#include <QDebug>


TranslationInfoParser::TranslationInfoParser(DummyTranslation* translation)
    : _translation(translation)
{
}


bool TranslationInfoParser::startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& attrs)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    if (qName == "book")
    {
        _bookInfo.code = attrs.value("code");
        _bookInfo.name = attrs.value("name");
        _bookInfo.chapterSize.clear();
    }

    if (qName == "chapter")
    {
        _bookInfo.chapterSize.append(attrs.value("verses").toInt());
    }

    return true;
}

bool TranslationInfoParser::endElement(const QString& namespaceURI, const QString& localName, const QString &qName)
{
    Q_UNUSED(namespaceURI);
    Q_UNUSED(localName);

    if (qName == "book")
        _translation->addBookInfo(_bookInfo);

    return true;
}
