#include "PlaceAccesser.h"

#include "Settings.h"


QString PlaceAccesser::bookCode(const Place& place)
{
    return place.bookCode();
}

int PlaceAccesser::chapterNo(const Place& place)
{
    return place.chapterNo();
}

QList<int> PlaceAccesser::verses(const Place& place)
{
    return place.verses().toList();
}


QString PlaceAccesser::string(const Place& place)
{
    return place.toString(Settings::instance()->translation());
}


Place PlaceAccesser::place(const QString& bookCode, int chapterNo)
{
    return Place(bookCode, chapterNo);
}

Place PlaceAccesser::placeOneVerse(const QString& bookCode, int chapterNo, int verse)
{
    QSet<int> verses;
    verses << verse;
    return Place(bookCode, chapterNo, verses);
}
