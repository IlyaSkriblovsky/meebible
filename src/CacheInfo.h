#pragma once

#include <QAbstractListModel>
#include <QList>


class Translation;
class Language;


class CacheInfo: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int count READ count NOTIFY countChanged)

    public:
        enum Roles
        {
            TranslationRole = Qt::UserRole + 1,
            LanguageRole,
            DataSizeRole,
            IndexSizeRole,
        };


        CacheInfo();
        ~CacheInfo();


        virtual int rowCount(const QModelIndex& index = QModelIndex()) const;
        virtual QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const;


    signals:
        void countChanged();


    public slots:
        void update();

        int count();

        void deleteTranslation(QString transCode, QString langCode);
        void deleteIndexOnly(QString transCode, QString langCode);


    private:
        struct CacheItem
        {
            Translation* translation;
            Language* language;
            int dataSize;
            int indexSize;
        };

        QList<CacheItem> _items;
};
