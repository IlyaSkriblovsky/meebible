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
    Q_PROPERTY(QString fontName READ fontName WRITE setFontName NOTIFY fontNameChanged)
    Q_PROPERTY(float lineSpacing READ lineSpacing WRITE setLineSpacing NOTIFY lineSpacingChanged)

    Q_PROPERTY(int scrollPos READ scrollPos WRITE setScrollPos NOTIFY scrollPosChanged)

    Q_PROPERTY(int fullscreen READ fullscreen WRITE setFullscreen NOTIFY fullscreenChanged)

    Q_PROPERTY(bool searchNoticeShown READ searchNoticeShown WRITE setSearchNoticeShown)

    Q_PROPERTY(bool inverted READ inverted WRITE setInverted NOTIFY invertedChanged)

    Q_PROPERTY(QString webService READ webService)

    public:
        Settings(Languages* langs, QObject* parent = 0);
        ~Settings();

        static Settings* instance() { return _instance; }

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

        QString fontName() const;
        void setFontName(const QString& name);

        float lineSpacing() const;
        void setLineSpacing(float spacing);

        int scrollPos() const;
        void setScrollPos(int pos);

        bool fullscreen() const;
        void setFullscreen(bool fs);

        bool searchNoticeShown() const;
        void setSearchNoticeShown(bool shown);


        bool inverted() const { return _inverted; }
        void setInverted(bool inverted);


        QString webService() const { return _webService; }


    signals:
        void languageChanged();
        void translationChanged();

        void bookCodeChanged();
        void chapterNoChanged();

        void floatingHeaderChanged();

        void fontSizeChanged();
        void fontNameChanged();
        void lineSpacingChanged();

        void scrollPosChanged();

        void fullscreenChanged();

        void invertedChanged();



    private slots:
        void onLanguagesLoadingFinished();


    private:
        static Settings* _instance;

        QSettings _settings;

        Languages* _languages;

        QString _bookCode;
        int _chapterNo;

        bool _floatingHeader;

        int _fontSize;
        QString _fontName;
        float _lineSpacing;

        int _scrollPos;

        bool _fullscreen;

        bool _searchNoticeShown;

        bool _inverted;

        QString _transCode;
        QString _langCode;


        QString _webService;
};

#endif // SETTINGS_H
