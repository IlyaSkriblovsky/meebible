#ifndef TRANSLATIONINFOPARSER_H
#define TRANSLATIONINFOPARSER_H

#include <QXmlDefaultHandler>

#include "Translation.h"

class TranslationInfoParser: public QXmlDefaultHandler
{
    public:
        TranslationInfoParser(Translation* translation);

        virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& attrs);
        virtual bool endElement(const QString& namespaceURI, const QString& localName, const QString& qName);


    private:
        Translation* _translation;

        Translation::BookInfo _bookInfo;
};

#endif
