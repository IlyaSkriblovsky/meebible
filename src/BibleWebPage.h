#ifndef BIBLEWEBPAGE_H
#define BIBLEWEBPAGE_H

#include <QWebPage>

class BibleWebPage : public QWebPage
{
public:
    BibleWebPage(QObject* page = 0);

protected:
    virtual void javaScriptConsoleMessage(const QString &message, int lineNumber, const QString &sourceID);
};

#endif // BIBLEWEBPAGE_H
