#pragma once

#include "Place.h"

struct SearchResult
{
    Place place;
    QString preview;
    int matchCount;

    SearchResult(): matchCount(0) { }
    SearchResult(const Place& place, const QString& preview, int matchCount)
        : place(place), preview(preview), matchCount(matchCount)
    {
    }
};

Q_DECLARE_METATYPE(SearchResult);
