#include "Bookmarks.h"

#include <QDebug>

#include "Place.h"
#include "Settings.h"


Q_DECLARE_METATYPE(QList<int>)


Bookmarks* Bookmarks::_instance = 0;


Bookmarks::Bookmarks()
{
    if (_instance)
        qCritical() << "Second instance of Bookmarks";

    _instance = this;

    Bookmark b;
    b.title = "Title";

    QSet<int> verses;
    verses << 18 << 19;
    b.place = Place("pr", 5, verses);
    b.text = "text text";
    _bookmarks << b;

    b.title = "Title 2";
    verses.clear();
    verses << 2 << 3 << 5;
    b.place = Place("re", 21, verses);
    _bookmarks << b;


    QHash<int, QByteArray> roleNames;
    roleNames[Qt::DisplayRole] = "title";
    roleNames[TextRole] = "subtitle";
    roleNames[PlaceRole] = "place";
    roleNames[PlaceTextRole] = "placeText";
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
            QString placeText;
            if (Settings::instance()->translation())
                placeText = bookmark.place.toString(Settings::instance()->translation());

            if (bookmark.title.isEmpty())
                return placeText;
            else
                return QString("%1 | %2")
                    .arg(bookmark.title)
                    .arg(placeText);
        }

        case PlaceTextRole:
        {
            QString placeText;
            if (Settings::instance()->translation())
                placeText = bookmark.place.toString(Settings::instance()->translation());

            return placeText;
        }

        case TitleRole:
            return bookmark.title;

        case TextRole:
            return bookmark.text;


        case PlaceRole:
            return QVariant::fromValue(bookmark.place);
    }

    return QVariant();
}



void Bookmarks::addBookmark(const Place& place, const QString& title, const QString& text)
{
    beginInsertRows(QModelIndex(), _bookmarks.size(), _bookmarks.size());

    Bookmark b;
    b.place = place;
    b.title = title;
    b.text = text;

    _bookmarks.append(b);
    endInsertRows();
}
