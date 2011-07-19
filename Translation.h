#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QObject>
#include <QString>


class ChapterRequest;
class Language;


class Translation: public QObject
{
    Q_OBJECT

public:
    virtual ~Translation();

    virtual const Language* language() const = 0;

    virtual QString code() const = 0;
    virtual QString name() const = 0;

    virtual QString bookName(const QString& bookCode) const = 0;
    virtual QList<QString> bookCodes() const = 0;
    virtual bool hasBook(const QString& bookCode) const = 0;

    virtual int chaptersInBook(const QString& bookCode) const = 0;
    virtual int versesInChapter(const QString& bookCode, int chapterNo) const = 0;

    virtual ChapterRequest* requestChapter(const QString& bookCode, int chapterNo) = 0;
};

#endif // TRANSLATION_H
