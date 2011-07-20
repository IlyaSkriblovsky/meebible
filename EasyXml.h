#ifndef EASYXML_H
#define EASYXML_H

#include <QString>
#include <QDomDocument>
#include <QMap>
#include <QList>
#include <QPair>

class EasyElement
{
    public:
        friend EasyElement* tag(const QString& tagname);

        ~EasyElement();

        void del();

        EasyElement* append(EasyElement* child);
        EasyElement* text(const QString& text);
        EasyElement* attr(const QString& name, const QString& value);

        EasyElement* appendTo(QDomDocument& doc);

    private:
        QString _tagname;
        QMap<QString, QString> _attrs;
        QList<QPair<EasyElement*, QString> > _childs;

        EasyElement(const EasyElement&);
        EasyElement(const QString& tagname);

        QDomElement element(QDomDocument& doc);
};


EasyElement* tag(const QString& tagname);

#endif // EASYXML_H
