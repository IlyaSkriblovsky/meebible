#include <QCoreApplication>

#include <QDebug>

#include <QXmlSimpleReader>
#include <QXmlDefaultHandler>

class Handler: public QXmlDefaultHandler
{
    public:
        virtual bool startElement(const QString& namespaceURI, const QString& localName, const QString& qName, const QXmlAttributes& attrs)
        {
            qDebug() << namespaceURI << localName << qName;

            for (int i = 0; i < attrs.count(); i++)
                qDebug() << "\t" << attrs.qName(i) << attrs.value(i);

            return true;
        }

        virtual bool fatalError(const QXmlParseException& exception)
        {
            qWarning() << "error" << exception.lineNumber() << ":" << exception.columnNumber() << exception.message();
            return false;
        }
};

int main(int argc, char *argv[])
{
    QCoreApplication app(argc, argv);

    QXmlSimpleReader reader;
    QFile file("meta.xml");
    QXmlInputSource *source = new QXmlInputSource(&file);

    Handler *handler = new Handler;
    reader.setContentHandler(handler);
    reader.setErrorHandler(handler);
    qDebug() << reader.parse(source);
}
