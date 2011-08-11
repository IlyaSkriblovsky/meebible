#include "Settings.h"

#include "Language.h"
#include "Languages.h"
#include "Translation.h"


Settings::Settings(Languages* langs, QObject* parent):
    QObject(parent)
{
    _language = langs->langByCode(_settings.value("General/langCode").toString());

    if (_language == 0)
        _translation = 0;
    else
    {
        _translation = _language->translationByCode(
            _settings.value("General/transCode").toString()
        );
    }


    _bookCode = _settings.value("General/bookCode").toString();
    _chapterNo = _settings.value("General/chapterNo").toInt();
}


Settings::~Settings()
{
    _settings.setValue("General/langCode", _language ? _language->code() : "");
    _settings.setValue("General/transCode", _translation ? _translation->code() : "");

    _settings.setValue("General/bookCode", _bookCode);
    _settings.setValue("General/chapterNo", _chapterNo);
}



Language* Settings::language() const
{
    return _language;
}

void Settings::setLanguage(Language* lang)
{
    if (_language == lang) return;

    _language = lang;
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
    chapterNoChanged();
}
