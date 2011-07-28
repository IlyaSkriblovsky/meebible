#ifndef TRANSLATIONSLIST_H
#define TRANSLATIONSLIST_H

#include <QList>
#include <QPair>
#include <QVariantList>


class Language;
class Translation;

class TranslationsList: public QObject
{
    Q_OBJECT

public:
    TranslationsList();
    ~TranslationsList();

    void addTranslation(const Language *lang, Translation *translation);

    QList<Translation*> translationsForLang(const Language* lang);

    Q_INVOKABLE QVariantList translationsForLang_js(Language* lang);


private:
    QList<QPair<const Language *, Translation *> > _translations;
};

#endif // TRANSLATIONSLIST_H
