#include "Indexer.h"

#include <QDebug>

#include "Translation.h"
#include "Paths.h"
#include "StringTokenizer.h"


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



void Indexer::onDocumentToken(void* data, const QString& token, int startPos, int endPos, int number)
{
    Q_UNUSED(startPos); Q_UNUSED(endPos);

    AddDocumentClosure* closure = static_cast<AddDocumentClosure*>(data);

    closure->index->add(token.toUtf8().constData(), MatchEntry(closure->docid, number));
}


void Indexer::addDocument(const QString& content, int docid)
{
    if (_indexFile == 0)
    {
        qCritical() << "Indexer::_indexFile == 0";
        return;
    }

    AddDocumentClosure closure;
    closure.index = _indexFile;
    closure.docid = docid;

    StringTokenizer::tokenize(content, onDocumentToken, &closure);
}


void Indexer::onQueryToken(void* data, const QString& token, int startPos, int endPos, int number)
{
    Q_UNUSED(startPos); Q_UNUSED(endPos); Q_UNUSED(number);

    QueryParseClosure* closure = static_cast<QueryParseClosure*>(data);
    closure->tokens.append(token);
}


QSet<int> Indexer::search(const QString& query)
{
    if (_indexFile == 0)
    {
        qCritical() << "Indexer::_indexFile == 0";
        return QSet<int>();
    }

    QueryParseClosure closure;
    StringTokenizer::tokenize(query, onQueryToken, &closure);

    QSet<int> result;

    for (int i = 0; i < closure.tokens.size(); i++)
    {
        std::list<MatchEntry> entries = _indexFile->search(closure.tokens[i].toUtf8());
        for (std::list<MatchEntry>::const_iterator i = entries.begin(); i != entries.end(); i++)
            result.insert(i->docid);
    }

    return result;
}
