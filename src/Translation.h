#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QStringList>

#include "Place.h"


class QNetworkAccessManager;


class ChapterRequest;
class Language;


class Translation: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString code READ code)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString sourceUrl READ sourceUrl NOTIFY sourceUrlChanged)
    Q_PROPERTY(QString copyright READ copyright NOTIFY copyrightChanged)
    Q_PROPERTY(bool rtl READ rtl NOTIFY rtlChanged)

    Q_PROPERTY(bool loading READ loading NOTIFY loadingChanged)
    Q_PROPERTY(bool loaded READ loaded NOTIFY loadedChanged)

    public:
        enum Roles {
            BookCodeRole = Qt::UserRole + 1,
            BookNameRole,
            VerseCountsRole
        };


        struct BookInfo
        {
            QString code;
            QString name;
            QList<int> chapterSize;
        };


        Translation(
            const QString& code,
            Language* language,
            const QString& name,
            const QString& sourceUrl,
            const QString& copyright,
            bool rtl
        );

        const Language* language() const { return _language; }

        QString code() const { return _code; }
        QString name() const { return _name; }
        QString sourceUrl() const { return _sourceUrl; }
        QString copyright() const { return _copyright; }
        bool rtl() const { return _rtl; }



        Q_INVOKABLE virtual QString bookName(const QString& bookCode) const;
        Q_INVOKABLE virtual QStringList bookCodes() const;
        bool hasBook(const QString& bookCode) const;

        Q_INVOKABLE virtual int chaptersInBook(const QString& bookCode) const;
        Q_INVOKABLE virtual int versesInChapter(const QString& bookCode, int chapterNo) const;

        virtual QList<int> verseCounts(const QString& bookCode) const;

        virtual ChapterRequest* requestChapter(QNetworkAccessManager* nam, const Place& place);


        virtual int rowCount(const QModelIndex& index = QModelIndex()) const;
        virtual QVariant data(const QModelIndex& index, int role) const;


        Q_INVOKABLE QString bookCodeAt(int row) const;


        void loadFromXML(const QString& xml);

        bool loading() { return _loading; }

        bool loaded() { return _bookCodes.size() > 0; }


    public slots:
        void reload(bool useCache);


    signals:
        void nameChanged();
        void sourceUrlChanged();
        void copyrightChanged();
        void rtlChanged();

        void loadingFinished();
        void loadingError();
        void loadingChanged();
        void loadedChanged();


    private slots:
        void translationXMLReceived();


    private:
        QString _code;

        Language* _language;

        QString _name;
        QString _sourceUrl;
        QString _copyright;
        bool _rtl;

        QMap<QString, BookInfo> _books;
        QStringList _bookCodes;


        bool _loading;

        void setLoading(bool loading);


        void addBookInfo(const BookInfo& bookInfo);

        friend class TranslationInfoParser;
};

#endif // TRANSLATION_H
