#include "Indexer.h"

#include <cstring>

#include <QDebug>
#include <QMultiMap>
#include <QPair>

#include "Translation.h"
#include "Paths.h"
#include "StringTokenizer.h"
#include "SearchQueryParser.h"


Indexer::Indexer(QObject* parent/* = 0*/)
    : QObject(parent), _indexFile(0), _translation(0)
{
}

Indexer::~Indexer()
{
    delete _indexFile;
}


void Indexer::setTranslation(const Translation* translation)
{
    if (_translation == translation) return;

    _translation = translation;

    delete _indexFile;

    if (_translation)
        _indexFile = new IndexTree(Paths::indexFile(_translation).toUtf8().constData());
    else
        _indexFile = 0;
}


void Indexer::clear()
{
    if (_indexFile)
        _indexFile->clear();
}

void Indexer::onDocumentToken(void* data, const QString& token, int startPos, int endPos, int number)
{
    Q_UNUSED(endPos);

    AddChapterClosure* closure = static_cast<AddChapterClosure*>(data);

    closure->index->add(token.toUtf8().constData(), MatchEntry(
        closure->bookNo,
        closure->chapterNo,
        number,
        startPos
    ));
}


void Indexer::addChapter(int bookNo, int chapterNo, const QString& content)
{
    if (_indexFile == 0)
    {
        qCritical() << "Indexer::_indexFile == 0";
        return;
    }

    AddChapterClosure closure;
    closure.index = _indexFile;
    closure.bookNo = bookNo;
    closure.chapterNo = chapterNo;

    StringTokenizer::tokenize(content, onDocumentToken, &closure);
    _indexFile->header()->docCount++;
}


int Indexer::docCount()
{
    if (_indexFile == 0)
        return 0;
    return _indexFile->header()->docCount;
}


void Indexer::onQueryToken(void* data, const QString& token, int startPos, int endPos, int number)
{
    Q_UNUSED(startPos); Q_UNUSED(endPos); Q_UNUSED(number);

    QueryParseClosure* closure = static_cast<QueryParseClosure*>(data);
    closure->tokens.append(token);
}


typedef QPair<int, int> Chapter;
typedef QPair<MatchEntry, int> TokenMatch;

QMap<Chapter, QList<MatchEntry> > Indexer::search(const QString& query, int cutoffDistance)
{
    if (_indexFile == 0)
    {
        qCritical() << "Indexer::_indexFile == 0";
        return QMap<Chapter, QList<MatchEntry> >();
    }

    QList<SearchQueryParser::QueryToken> queryTokens = SearchQueryParser::parseQuery(query);
    int queryTokensN = queryTokens.size();

    // [BookNo, ChapterNo] => [TokenNo, MatchEntry]
    QMultiMap<Chapter, TokenMatch> byChapter;

    for (int ti = 0; ti < queryTokensN; ti++)
    {
        std::list<MatchEntry> tokenMatches = queryTokens[ti].prefix ?
            _indexFile->search_prefix(queryTokens[ti].token.toUtf8())
                :
            _indexFile->search(queryTokens[ti].token.toUtf8());

        for (std::list<MatchEntry>::iterator i = tokenMatches.begin(); i != tokenMatches.end(); i++)
            byChapter.insert(Chapter(i->bookNo, i->chapterNo), TokenMatch(*i, ti));
    }

    QMap<QPair<int, int>, QList<MatchEntry> > result;

    foreach (const Chapter& chapter, byChapter.uniqueKeys())
    {
        QList<TokenMatch> values = byChapter.values(chapter);
        qSort(values); // sorting by MatchEntry (tokenNo)

        bool hasToken[queryTokensN];
        memset(hasToken, 0, sizeof(hasToken));

        int prevTokenNo = -1;
        int groupStart = 0;

        for (int i = 0; i <= values.size(); i++)
        {
            if (i == values.size() || (prevTokenNo != -1 && values[i].first.tokenNo - prevTokenNo > cutoffDistance))
            {
                bool all = true;
                for (int j = 0; j < queryTokensN; j++)
                    if (! hasToken[j])
                    {
                        all = false;
                        break;
                    }

                if (all)
                {
                    for (int j = groupStart; j < i; j++)
                    {
                        QList<MatchEntry>& list = result[chapter];
                        list.append(values[j].first);
                    }
                }

                memset(hasToken, 0, sizeof(hasToken));
                groupStart = i;
            }

            if (i < values.size())
            {
                hasToken[values[i].second] = true;
                prevTokenNo = values[i].first.tokenNo;
            }
        }
    }

    return result;
}
