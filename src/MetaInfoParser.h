#ifndef METAINFOPARSER_H
#define METAINFOPARSER_H

#include <QXmlDefaultHandler>

class Languages;

class MetaInfoParser: public QXmlDefaultHandler
{
    public:
        MetaInfoParser(Languages* languages);

        virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& attrs);


    private:
        Languages* _languages;

        QString _transCode;
        QString _transSourceUrl;
        QString _transCopyright;
        bool _transRtl;
};

#endif
