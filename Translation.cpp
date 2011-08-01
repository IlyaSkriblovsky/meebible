#include "Translation.h"

#include <QHash>
#include <QByteArray>



Translation::Translation()
{
    QHash<int, QByteArray> roleNames;
    roleNames[Qt::DisplayRole] = "value";
    roleNames[BookCodeRole] = "bookCode";
    roleNames[BookNameRole] = "bookName";
    setRoleNames(roleNames);
}

Translation::~Translation()
{
}



int Translation::rowCount(const QModelIndex& index) const
{
    Q_UNUSED(index)

    return bookCodes().size();
}

QVariant Translation::data(const QModelIndex& index, int role) const
{
    QStringList codes = bookCodes();

    if (index.row() < 0  ||  index.row() >= codes.size())
        return QVariant();

    switch (role)
    {
        case BookCodeRole: return codes.at(index.row());

        case Qt::DisplayRole:
        case BookNameRole:
            return bookName(codes.at(index.row()));
    }

    return QVariant();
}
