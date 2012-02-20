#include "Bookmarks.h"

#include <QDebug>

#include "Place.h"
#include "Settings.h"


Bookmarks* Bookmarks::_instance = 0;


Bookmarks::Bookmarks()
{
    if (_instance)
        qCritical() << "Second instance of Bookmarks";

    _instance = this;

    Bookmark b;
    b.title = "Title";
    b.bookCode = "pr";
    b.chapterNo = 5;
    b.verses << 18 << 19;
    b.text = "text text";
    _bookmarks << b;

    b.title = "Title 2";
    _bookmarks << b;


    QHash<int, QByteArray> roleNames;
    roleNames[Qt::DisplayRole] = "title";
    roleNames[TextRole] = "subtitle";
    roleNames[PlaceRole] = "place";
    setRoleNames(roleNames);
}


int Bookmarks::rowCount(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return _bookmarks.size();
}


QVariant Bookmarks::data(const QModelIndex& index, int role) const
{
    if (index.row() < 0 || index.row() >= _bookmarks.size())
        return QVariant();

    Bookmark bookmark = _bookmarks.at(index.row());

    switch (role)
    {
        case Qt::DisplayRole:
        {
            Place place(bookmark.bookCode, bookmark.chapterNo, bookmark.verses);
            QString placeStr;
            if (Settings::instance()->translation())
                placeStr = place.toString(Settings::instance()->translation());

            if (bookmark.title.isEmpty())
                return placeStr;
            else
                return QString("%1 | %2")
                    .arg(bookmark.title)
                    .arg(placeStr);
        }

        case PlaceRole:
        {
            Place place(bookmark.bookCode, bookmark.chapterNo, bookmark.verses);
            QString placeStr;
            if (Settings::instance()->translation())
                placeStr = place.toString(Settings::instance()->translation());

            return placeStr;
        }

        case TitleRole:
            return bookmark.title;

        case TextRole:
            return bookmark.text;
    }

    return QVariant();
}



void Bookmarks::addBookmark(const QString& bookCode, int chapterNo, const QSet<int>& verses, const QString& title, const QString& text)
{
    beginInsertRows(QModelIndex(), _bookmarks.size(), _bookmarks.size());

    Bookmark b;
    b.bookCode = bookCode;
    b.chapterNo = chapterNo;
    b.verses = verses;
    b.title = title;
    b.text = text;

    _bookmarks.append(b);
    endInsertRows();
}
