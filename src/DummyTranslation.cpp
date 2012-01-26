#include "DummyTranslation.h"


DummyTranslation::DummyTranslation(
    const QString& code,
    Language* language,
    const QString& name,
    const QString& sourceUrl,
    const QString& copyright
)
    : _code(code), _language(language), _name(name),
      _sourceUrl(sourceUrl), _copyright(copyright)
{
}

DummyTranslation::~DummyTranslation()
{
}
