#pragma once

#include <QThread>
#include <QList>

class Translation;
class Indexer;


struct SearchMatch
{
    int bookNo;
    int chapterNo;
    int offset;
    int matchesInChapter;
};


class SearchThread: public QThread
{
    Q_OBJECT

    public:
        SearchThread(
            Indexer* indexer,
            const Translation* translation, const QString& query,
            int maxResults,
            QObject* parent = 0
        );

        const Translation* translation() const { return _translation; }

        const QList<SearchMatch> results() const { return _results; }
        QString query() const { return _query; }

    signals:
        void finished();

    protected:
        virtual void run();

    private:
        Indexer* _indexer;
        const Translation* _translation;
        QString _query;
        int _maxResults;

        QList<SearchMatch> _results;
};
