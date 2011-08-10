#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

class Language;
class Languages;
class Translation;


class Settings: public QObject
{
    Q_OBJECT

    Q_PROPERTY(Language* language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(Translation* translation READ translation WRITE setTranslation NOTIFY translationChanged)

    public:
        Settings(Languages* langs, QObject* parent = 0);
        ~Settings();

        Language* language() const;
        void setLanguage(Language* lang);

        Translation* translation() const;
        void setTranslation(Translation* translation);


    signals:
        void languageChanged();
        void translationChanged();


    private:
        QSettings _settings;

        Language* _language;
        Translation* _translation;
};

#endif // SETTINGS_H
