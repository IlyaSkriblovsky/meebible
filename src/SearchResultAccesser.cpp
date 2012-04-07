#include "SearchResultAccesser.h"

Place SearchResultAccesser::place(const SearchResult& result)
{
    return result.place;
}

QString SearchResultAccesser::preview(const SearchResult& result)
{
    return result.preview;
}

int SearchResultAccesser::matchCount(const SearchResult& result)
{
    return result.matchCount;
}
