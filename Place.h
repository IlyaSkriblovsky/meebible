#ifndef PLACE_H
#define PLACE_H

#include <QString>
#include <QSet>


class Translation;


class Place
{
public:
    Place(const QString& bookCode, int chapterNo, const QSet<int>& verses = QSet<int>());

    QString toString(const Translation* translation) const;

private:
    QString _bookCode;
    int _chapterNo;
    QSet<int> _verses;
};

#endif // PLACE_H
