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



int Translation::chaptersInBook(const QString& bookCode) const
{
    return verseCounts(bookCode).size();
}

int Translation::versesInChapter(const QString& bookCode, int chapterNo) const
{
    return verseCounts(bookCode).at(chapterNo - 1);
}



QString Translation::bookCodeAt(int row) const
{
    return bookCodes().at(row);
}



bool Translation::hasBook(const QString& bookCode) const
{
    return bookCodes().indexOf(bookCode) != -1;
}



bool Translation::rtl() const
{
    return false;
}
