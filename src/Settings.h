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

    Q_PROPERTY(QString bookCode READ bookCode WRITE setBookCode NOTIFY bookCodeChanged)
    Q_PROPERTY(int chapterNo READ chapterNo WRITE setChapterNo NOTIFY chapterNoChanged)

    Q_PROPERTY(bool floatingHeader READ floatingHeader WRITE setFloatingHeader NOTIFY floatingHeaderChanged)

    Q_PROPERTY(int fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)

    public:
        Settings(Languages* langs, QObject* parent = 0);
        ~Settings();

        Language* language() const;
        void setLanguage(Language* lang);

        Translation* translation() const;
        void setTranslation(Translation* translation);


        QString bookCode() const;
        void setBookCode(const QString& bookcode);

        int chapterNo() const;
        void setChapterNo(int chapterNo);

        bool floatingHeader() const;
        void setFloatingHeader(bool show);

        int fontSize() const;
        void setFontSize(int size);


    signals:
        void languageChanged();
        void translationChanged();

        void bookCodeChanged();
        void chapterNoChanged();

        void floatingHeaderChanged();

        void fontSizeChanged();


    private:
        QSettings _settings;

        Language* _language;
        Translation* _translation;

        QString _bookCode;
        int _chapterNo;

        bool _floatingHeader;

        int _fontSize;
};

#endif // SETTINGS_H
