TRANSLATIONS += \
    translations/meebible_ru.ts

COMPILEDTRANSLATIONS = \
    translations/meebible_ru.qm

symbian {
    translations.sources = $$COMPILEDTRANSLATIONS
    translations.path = translations
    DEPLOYMENT += translations
} else {
    translations.files = $$COMPILEDTRANSLATIONS
    translations.path = $$INSTALLDIR/translations
    INSTALLS += translations
}
