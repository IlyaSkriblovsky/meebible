#ifndef TRANSLATIONSLIST_H
#define TRANSLATIONSLIST_H

#include <QList>
#include <QPair>
#include <QVariantList>
#include <QAbstractListModel>


class Language;
class Translation;

class TranslationsList: public QAbstractListModel
{
    Q_OBJECT

public:
    enum Roles {
        LanguageRole = Qt::UserRole + 1,
        TranslationRole,
        LanguageCodeRole,
        TranslationNameRole
    };

    TranslationsList();
    ~TranslationsList();

    void addTranslation(Language *lang, Translation *translation);

    QList<Translation*> translationsForLang(const Language* lang);


    virtual int rowCount(const QModelIndex& index = QModelIndex()) const;
    virtual QVariant data(const QModelIndex& index, int role) const;


private:
    QList<QPair<Language *, Translation *> > _translations;
};

#endif // TRANSLATIONSLIST_H
