#include "Settings.h"

#include "Language.h"
#include "Languages.h"
#include "Translation.h"


Settings::Settings(Languages* langs, QObject* parent):
    QObject(parent)
{
    _language = langs->langByCode(_settings.value("General/langCode", "e").toString());

    if (_language == 0)
        _translation = 0;
    else
    {
        _translation = _language->translationByCode(
            _settings.value("General/transCode", "nwt").toString()
        );
    }


    _bookCode = _settings.value("General/bookCode", "ge").toString();
    _chapterNo = _settings.value("General/chapterNo", 1).toInt();

    _floatingHeader = _settings.value("General/floatingHeader", true).toBool();
}

Settings::~Settings()
{
}



Language* Settings::language() const
{
    return _language;
}

void Settings::setLanguage(Language* lang)
{
    if (_language == lang) return;

    _language = lang;
    _settings.setValue("General/langCode", _language ? _language->code() : "");
    languageChanged();
}


Translation* Settings::translation() const
{
    return _translation;
}

void Settings::setTranslation(Translation* translation)
{
    if (_translation == translation) return;

    _translation = translation;
    _settings.setValue("General/transCode", _translation ? _translation->code() : "");
    translationChanged();
}


QString Settings::bookCode() const
{
    return _bookCode;
}

void Settings::setBookCode(const QString& bookCode)
{
    if (_bookCode == bookCode) return;

    _bookCode = bookCode;
    _settings.setValue("General/bookCode", _bookCode);
    bookCodeChanged();
}


int Settings::chapterNo() const
{
    return _chapterNo;
}

void Settings::setChapterNo(int chapterNo)
{
    if (_chapterNo == chapterNo) return;

    _chapterNo = chapterNo;
    _settings.setValue("General/chapterNo", _chapterNo);
    chapterNoChanged();
}



bool Settings::floatingHeader() const
{
    return _floatingHeader;
}

void Settings::setFloatingHeader(bool show)
{
    if (_floatingHeader != show)
    {
        _floatingHeader = show;
        _settings.setValue("General/floatingHeader", _floatingHeader);
        floatingHeaderChanged();
    }
}
