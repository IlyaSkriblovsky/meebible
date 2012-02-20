#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QAbstractListModel>
#include <QList>
#include <QSet>


class Bookmarks: public QAbstractListModel
{
    Q_OBJECT

    public:
        static Bookmarks* instance() { return _instance; }

        enum Roles {
            PlaceRole = Qt::UserRole + 1,
            TitleRole,
            TextRole
        };


        Bookmarks();

        virtual int rowCount(const QModelIndex& index = QModelIndex()) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;


        void addBookmark(
            const QString& bookCode,
            int chapterNo,
            const QSet<int>& verses,
            const QString& title,
            const QString& text
        );


    private:
        static Bookmarks* _instance;

        struct Bookmark
        {
            QString bookCode;
            int chapterNo;
            QSet<int> verses;

            QString title;
            QString text;
        };


        QList<Bookmark> _bookmarks;
};

#endif
