#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QAbstractListModel>
#include <QString>
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

public:
    enum Roles {
        BookCodeRole = Qt::UserRole + 1,
        BookNameRole
    };


    Translation();
    virtual ~Translation();

    virtual const Language* language() const = 0;

    virtual QString code() const = 0;
    virtual QString name() const = 0;

    Q_INVOKABLE virtual QString bookName(const QString& bookCode) const = 0;
    Q_INVOKABLE virtual QStringList bookCodes() const = 0;
    virtual bool hasBook(const QString& bookCode) const = 0;

    virtual int chaptersInBook(const QString& bookCode) const = 0;
    virtual int versesInChapter(const QString& bookCode, int chapterNo) const = 0;

    virtual ChapterRequest* requestChapter(QNetworkAccessManager* nam, const QString& bookCode, int chapterNo) = 0;


    virtual int rowCount(const QModelIndex& index = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role) const;
};

#endif // TRANSLATION_H
