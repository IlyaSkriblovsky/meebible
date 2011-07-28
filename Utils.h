#ifndef UTILS_H
#define UTILS_H

namespace Utils
{
    template<typename T>
    QVariantList objectListToVariantList(QList<T*> list)
    {
        QVariantList varlist;
        for (int i = 0; i < list.size(); i++)
            varlist.append(QVariant::fromValue((QObject*)list[i]));
        return varlist;
    }
}

#endif
