#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QAbstractListModel>
#include <QString>
#include <QList>
#include <QStringList>
#include <QVariantList>


class QNetworkAccessManager;


class ChapterRequest;
class Language;


class Translation: public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(QString code READ code)
    Q_PROPERTY(QString name READ name)
    Q_PROPERTY(QString sourceUrl READ sourceUrl)
    Q_PROPERTY(QString copyright READ copyright)

public:
    enum Roles {
        BookCodeRole = Qt::UserRole + 1,
        BookNameRole,
        VerseCountsRole
    };


    Translation();
    virtual ~Translation();

    virtual const Language* language() const = 0;

    virtual QString code() const = 0;
    virtual QString name() const = 0;
    virtual QString sourceUrl() const = 0;
    virtual QString copyright() const = 0;

    Q_INVOKABLE virtual QString bookName(const QString& bookCode) const = 0;
    Q_INVOKABLE virtual QStringList bookCodes() const = 0;
    bool hasBook(const QString& bookCode) const;

    Q_INVOKABLE virtual int chaptersInBook(const QString& bookCode) const;
    Q_INVOKABLE virtual int versesInChapter(const QString& bookCode, int chapterNo) const;

    virtual QList<int> verseCounts(const QString& bookCode) const = 0;

    virtual ChapterRequest* requestChapter(QNetworkAccessManager* nam, const QString& bookCode, int chapterNo) = 0;


    virtual int rowCount(const QModelIndex& index = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role) const;


    Q_INVOKABLE QString bookCodeAt(int row) const;
};

#endif // TRANSLATION_H
