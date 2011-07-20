#include "EasyXml.h"


EasyElement* tag(const QString& tagname)
{
    return new EasyElement(tagname);
}


    EasyElement::EasyElement(const QString& tagname)
    : _tagname(tagname)
{
}

EasyElement::~EasyElement()
{
    for (int i = 0; i < _childs.size(); i++)
        delete _childs[i].first;
}


void EasyElement::del()
{
    delete this;
}

EasyElement* EasyElement::append(EasyElement* child)
{
    _childs.append(QPair<EasyElement*, QString>(child, QString()));
    return this;
}

EasyElement* EasyElement::text(const QString& text)
{
    _childs.append(QPair<EasyElement*, QString>(0, text));
    return this;
}

EasyElement* EasyElement::attr(const QString& name, const QString& value)
{
    _attrs[name] = value;
    return this;
}

EasyElement* EasyElement::appendTo(QDomDocument& doc)
{
    doc.appendChild(element(doc));
    return this;
}


QDomElement EasyElement::element(QDomDocument& doc)
{
    QDomElement element = doc.createElement(_tagname);

    QMapIterator<QString, QString> i(_attrs);
    while (i.hasNext())
    {
        i.next();
        element.setAttribute(i.key(), i.value());
    }

    for (int i = 0; i < _childs.size(); i++)
    {
        QPair<EasyElement*, QString> pair = _childs[i];

        if (pair.first)
            element.appendChild(pair.first->element(doc));
        else
            element.appendChild(doc.createTextNode(pair.second));
    }

    return element;
}
