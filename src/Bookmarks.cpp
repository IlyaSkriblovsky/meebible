#include "Bookmarks.h"

#include <QDebug>
#include <QSettings>
#include <QStringList>

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


    QSettings settings;
    int size = settings.beginReadArray("Bookmarks");
    for (int i = 0; i < size; i++)
    {
        settings.setArrayIndex(i);

        QStringList vstr = settings.value("verses").toString().split(",");

        QSet<int> verses;
        foreach (QString v, vstr)
            verses << v.toInt();

        Bookmark b;
        b.place = Place(
            settings.value("bookCode").toString(),
            settings.value("chapterNo").toInt(),
            verses
        );
        b.text = settings.value("text").toString();

        _bookmarks << b;
    }
    settings.endArray();
}


Bookmarks::~Bookmarks()
{
    QSettings settings;

    settings.beginWriteArray("Bookmarks");
    for (int i = 0; i < _bookmarks.size(); i++)
    {
        settings.setArrayIndex(i);

        QStringList vstr;
        foreach (int v, _bookmarks.at(i).place.verses())
            vstr << QString::number(v);

        settings.setValue("bookCode", _bookmarks.at(i).place.bookCode());
        settings.setValue("chapterNo", _bookmarks.at(i).place.chapterNo());
        settings.setValue("verses", vstr.join(","));
        settings.setValue("text", _bookmarks.at(i).text);
    }
    settings.endArray();
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
