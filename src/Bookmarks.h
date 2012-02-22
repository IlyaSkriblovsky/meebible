#ifndef BOOKMARK_H
#define BOOKMARK_H

#include <QAbstractListModel>
#include <QList>
#include <QSet>

#include "Place.h"


class Bookmarks: public QAbstractListModel
{
    Q_OBJECT

    public:
        static Bookmarks* instance() { return _instance; }

        enum Roles {
            PlaceRole = Qt::UserRole + 1,
            PlaceTextRole,
            TextRole
        };


        Bookmarks();
        ~Bookmarks();

        virtual int rowCount(const QModelIndex& index = QModelIndex()) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;


        void addBookmark(
            const Place& place,
            const QString& text
        );


    private:
        static Bookmarks* _instance;

        struct Bookmark
        {
            Place place;

            QString text;
        };


        QList<Bookmark> _bookmarks;
};

#endif
