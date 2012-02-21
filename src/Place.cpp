#include "Place.h"

#include <QDebug>

#include "Translation.h"


Place::Place()
    : _chapterNo(0)
{
}

Place::Place(const QString& bookCode, int chapterNo, const QSet<int>& verses)
    : _bookCode(bookCode), _chapterNo(chapterNo), _verses(verses)
{
}


void Place::setBookCode(const QString& bookCode)
{
    _bookCode = bookCode;
}
void Place::setChapterNo(int chapterNo)
{
    _chapterNo = chapterNo;
}
void Place::setVerses(const QSet<int>& verses)
{
    _verses = verses;
}



QString Place::verseString() const
{
    QList<int> list = _verses.toList();
    qSort(list);

    QString verseString = "";

    int groupStart = -1;
    int prev = -1;

    for (int i = 0; i < list.size() + 1; ++i)
    {
        int cur;
        if (i < list.size()) cur = list[i];
        else cur = -1;

        if (prev == -1)
        {
            if (verseString.length() > 0) verseString += ",";
            verseString += QString::number(cur);
            prev = cur;
            groupStart = cur;
        }
        else if (cur == prev + 1)
        {
            prev = cur;
        }
        else
        {
            if (prev != groupStart) verseString += "-" + QString::number(prev);
            if (cur != -1) verseString += "," + QString::number(cur);
            prev = cur;
            groupStart = cur;
        }
    }

    return verseString;
}


QString Place::toString(const Translation* translation) const
{
    if (! isValid(translation))
        return QString();

    if (_verses.size() == 0)
        return QString("%1 %2")
                .arg(translation->bookName(_bookCode))
                .arg(_chapterNo);

    return QString("%1 %2:%3")
            .arg(translation->bookName(_bookCode))
            .arg(_chapterNo)
            .arg(verseString());
}

QString Place::toStringChapterOnly(const Translation* translation) const
{
    if (! isValid(translation))
        return QString();

    return QString("%1 %2")
            .arg(translation->bookName(_bookCode))
            .arg(_chapterNo);
}

QString Place::toStringCode() const
{
    if (_verses.size() == 0)
        return QString("%1 %2")
            .arg(_bookCode)
            .arg(_chapterNo);

    return QString("%1 %2:%3")
            .arg(_bookCode)
            .arg(_chapterNo)
            .arg(verseString());
}



Place Place::prevChapter(const Translation* translation) const
{
    if (_chapterNo > 1)
        return Place(_bookCode, _chapterNo - 1);

    QList<QString> bookCodes = translation->bookCodes();
    int thisIndex = bookCodes.indexOf(_bookCode);

    if (thisIndex == -1)
        return Place("xxx", 0);

    if (thisIndex == 0)
        return Place(_bookCode, _chapterNo);

    QString prevCode = bookCodes.at(thisIndex - 1);
    return Place(prevCode, translation->chaptersInBook(prevCode));
}

Place Place::nextChapter(const Translation* translation) const
{
    if (_chapterNo < translation->chaptersInBook(_bookCode))
        return Place(_bookCode, _chapterNo + 1);

    QList<QString> bookCodes = translation->bookCodes();
    int thisIndex = bookCodes.indexOf(_bookCode);

    if (thisIndex == -1)
        return Place("xxx", 0);

    if (thisIndex == bookCodes.size() - 1)
        return Place(_bookCode, _chapterNo);

    QString nextCode = bookCodes.at(thisIndex + 1);
    return Place(nextCode, 1);
}


bool Place::isValid(const Translation* translation) const
{
    if (! translation) return false;
    if (! translation->hasBook(_bookCode)) return false;

    if (_chapterNo < 1 || _chapterNo > translation->chaptersInBook(_bookCode)) return false;

    QList<int> list = _verses.toList();
    if (list.size() > 0)
    {
        qSort(list);
        if (list[0] < 1) return false;
        if (list[list.size() - 1] > translation->versesInChapter(_bookCode, _chapterNo)) return false;
    }

    return true;
}



bool Place::operator == (const Place& other)
{
    return  _bookCode == other._bookCode &&
            _chapterNo == other._chapterNo &&
            _verses == other._verses;
}

bool Place::sameChapter(const Place& other)
{
    return _bookCode == other._bookCode && _chapterNo == other._chapterNo;
}
