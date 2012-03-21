TRANSLATIONS += \
    translations/meebible_ru.ts \
    translations/meebible_es.ts

COMPILEDTRANSLATIONS = \
    translations/meebible_ru.qm \
    translations/meebible_es.qm

symbian {
    translations.sources = $$COMPILEDTRANSLATIONS
    translations.path = translations
    DEPLOYMENT += translations
} else {
    translations.files = $$COMPILEDTRANSLATIONS
    translations.path = $$INSTALLDIR/translations
    INSTALLS += translations
}
