#include "BibleWebPage.h"

#include <QDebug>


BibleWebPage::BibleWebPage(QObject* page): QWebPage(page)
{
}

void BibleWebPage::javaScriptConsoleMessage(const QString &msg, int lineNumber, const QString &sourceId)
{
    qDebug() << "JS:" << sourceId << lineNumber << ">" << msg;
}
