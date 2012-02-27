#include "PlaceAccesser.h"

#include "Settings.h"
#include "Translation.h"


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


QString PlaceAccesser::string(const Place& place, Translation* translation)
{
    return place.toString(translation);
}


Place PlaceAccesser::placeNoVerses(const QString& bookCode, int chapterNo)
{
    return Place(bookCode, chapterNo);
}

Place PlaceAccesser::placeOneVerse(const QString& bookCode, int chapterNo, int verse)
{
    QSet<int> verses;
    verses << verse;
    return Place(bookCode, chapterNo, verses);
}
