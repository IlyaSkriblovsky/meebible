#include "Place.h"

#include "Translation.h"


Place::Place(const QString& bookCode, int chapterNo, const QSet<int>& verses)
    : _bookCode(bookCode), _chapterNo(chapterNo), _verses(verses)
{
}


QString Place::toString(const Translation* translation) const
{
    if (_verses.size() == 0)
        return QString("%1 %2")
                .arg(translation->bookName(_bookCode))
                .arg(_chapterNo);

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

    return QString("%1 %2:%3")
            .arg(translation->bookName(_bookCode))
            .arg(_chapterNo)
            .arg(verseString);
}
