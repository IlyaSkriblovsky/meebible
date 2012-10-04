#ifndef PLACE_H
#define PLACE_H

#include <QString>
#include <QSet>
#include <QMetaType>


class Translation;


class Place
{
public:
    Place();
    Place(const QString& bookCode, int chapterNo, const QSet<int>& verses = QSet<int>());

    QString bookCode() const { return _bookCode; }
    int chapterNo() const { return _chapterNo; }
    QSet<int> verses() const { return _verses; }


    void setBookCode(const QString& code);
    void setChapterNo(int chapterNo);
    void setVerses(const QSet<int>& verses);


    QString toStringChapterOnly(const Translation* translation) const;

    QString toString(const Translation* translation) const;

    QString toStringCode() const;

    bool isValid(const Translation* translation) const;

    Place prevChapter(const Translation* translation) const;
    Place nextChapter(const Translation* translation) const;


    bool sameChapter(const Place& other);


    QString siteUrl(const Translation* translation) const;


    bool operator == (const Place& other) const;


private:
    QString _bookCode;
    int _chapterNo;
    QSet<int> _verses;

    QString verseString(bool dash = false) const;
};

Q_DECLARE_METATYPE(Place);

#endif // PLACE_H
