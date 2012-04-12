#pragma once

#include <QObject>

#include "SearchResult.h"

class SearchResultAccesser: public QObject
{
    Q_OBJECT

    public slots:
        Place place(const SearchResult& result);
        QString preview(const SearchResult& result);
        int matchCount(const SearchResult& result);
};
