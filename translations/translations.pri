TRANSLATIONS += \
    translations/meebible_ru.ts \
    translations/meebible_es.ts \
    translations/meebible_it.ts \
    translations/meebible_sv.ts

COMPILEDTRANSLATIONS = \
    translations/meebible_ru.qm \
    translations/meebible_es.qm \
    translations/meebible_it.qm \
    translations/meebible_sv.qm

symbian {
    translations.sources = $$COMPILEDTRANSLATIONS
    translations.path = translations
    DEPLOYMENT += translations
} else {
    translations.files = $$COMPILEDTRANSLATIONS
    translations.path = $$INSTALLDIR/translations
    INSTALLS += translations
}
