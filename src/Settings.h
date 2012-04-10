#ifndef SETTINGS_H
#define SETTINGS_H

#include <QSettings>

#include "Place.h"

class Language;
class Languages;
class Translation;


class Settings: public QObject
{
    Q_OBJECT

    Q_PROPERTY(Language* language READ language WRITE setLanguage NOTIFY languageChanged)
    Q_PROPERTY(Translation* translation READ translation WRITE setTranslation NOTIFY translationChanged)

    Q_PROPERTY(Place place READ place WRITE setPlace NOTIFY placeChanged)

    Q_PROPERTY(bool floatingHeader READ floatingHeader WRITE setFloatingHeader NOTIFY floatingHeaderChanged)

    Q_PROPERTY(double fontSize READ fontSize WRITE setFontSize NOTIFY fontSizeChanged)
    Q_PROPERTY(QString fontName READ fontName WRITE setFontName NOTIFY fontNameChanged)
    Q_PROPERTY(double lineSpacing READ lineSpacing WRITE setLineSpacing NOTIFY lineSpacingChanged)

    Q_PROPERTY(int scrollPos READ scrollPos WRITE setScrollPos NOTIFY scrollPosChanged)

    Q_PROPERTY(int fullscreen READ fullscreen WRITE setFullscreen NOTIFY fullscreenChanged)

    Q_PROPERTY(bool searchNoticeShown READ searchNoticeShown WRITE setSearchNoticeShown)

    Q_PROPERTY(bool inverted READ inverted WRITE setInverted NOTIFY invertedChanged)

    Q_PROPERTY(bool copyVerseNumbers READ copyVerseNumbers WRITE setCopyVerseNumbers NOTIFY copyVerseNumbersChanged)

    Q_PROPERTY(QString webService READ webService)

    Q_PROPERTY(bool gridPlaceSelector READ gridPlaceSelector WRITE setGridPlaceSelector NOTIFY gridPlaceSelectorChanged)

    public:
        Settings(Languages* langs, QObject* parent = 0);
        ~Settings();

        static Settings* instance() { return _instance; }

        Language* language() const;
        void setLanguage(Language* lang);

        Translation* translation() const;
        void setTranslation(Translation* translation);


        Place place() const;
        void setPlace(const Place& place);


        bool floatingHeader() const;
        void setFloatingHeader(bool show);

        double fontSize() const;
        void setFontSize(double size);

        QString fontName() const;
        void setFontName(const QString& name);

        double lineSpacing() const;
        void setLineSpacing(double spacing);

        int scrollPos() const;
        void setScrollPos(int pos);

        bool fullscreen() const;
        void setFullscreen(bool fs);

        bool searchNoticeShown() const;
        void setSearchNoticeShown(bool shown);


        bool inverted() const;
        void setInverted(bool inverted);


        bool copyVerseNumbers() const;
        void setCopyVerseNumbers(bool copyVerseNumbers);


        QString webService() const { return _webService; }

        bool gridPlaceSelector() const { return _gridPlaceSelector; }
        void setGridPlaceSelector(bool value);


    signals:
        void languageChanged();
        void translationChanged();

        void placeChanged();

        void floatingHeaderChanged();

        void fontSizeChanged();
        void fontNameChanged();
        void lineSpacingChanged();

        void scrollPosChanged();

        void fullscreenChanged();

        void invertedChanged();

        void copyVerseNumbersChanged();

        void gridPlaceSelectorChanged();



    private slots:
        void onLanguagesLoadingFinished();


    private:
        static Settings* _instance;

        QSettings _settings;

        Languages* _languages;

        Place _place;

        bool _floatingHeader;

        double _fontSize;
        QString _fontName;
        double _lineSpacing;

        int _scrollPos;

        bool _fullscreen;

        bool _searchNoticeShown;

        bool _inverted;

        bool _copyVerseNumbers;

        QString _transCode;
        QString _langCode;


        QString _webService;

        bool _gridPlaceSelector;
};

#endif // SETTINGS_H
