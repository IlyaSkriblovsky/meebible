#ifndef TRANSLATIONSLIST_H
#define TRANSLATIONSLIST_H

#include <QList>
#include <QPair>


class Language;
class Translation;

class TranslationsList
{
public:
    TranslationsList();

    void addTranslation(const Language *lang, Translation *translation);

    QList<Translation*> translationsForLang(const Language* lang);


private:
    QList<QPair<const Language *, Translation *> > _translations;
};

#endif // TRANSLATIONSLIST_H
