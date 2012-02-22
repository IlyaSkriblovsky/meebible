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
        case PlaceTextRole:
            return bookmark.place.toString(Settings::instance()->translation());

        case TextRole:
            return bookmark.text;


        case PlaceRole:
            return QVariant::fromValue(bookmark.place);
    }

    return QVariant();
}



void Bookmarks::addBookmark(const Place& place, const QString& text)
{
    beginInsertRows(QModelIndex(), _bookmarks.size(), _bookmarks.size());

    Bookmark b;
    b.place = place;
    b.text = text;

    _bookmarks.append(b);
    endInsertRows();
}
