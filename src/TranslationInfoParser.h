#ifndef TRANSLATIONINFOPARSER_H
#define TRANSLATIONINFOPARSER_H

#include <QXmlDefaultHandler>

#include "DummyTranslation.h"

class TranslationInfoParser: public QXmlDefaultHandler
{
    public:
        TranslationInfoParser(DummyTranslation* translation);

        virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& attrs);
        virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);


    private:
        DummyTranslation* _translation;

        DummyTranslation::BookInfo _bookInfo;
};

#endif
