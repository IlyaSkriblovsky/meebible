TRANSLATIONS += \
    translations/meebible_ru.ts

COMPILEDTRANSLATIONS = \
    translations/meebible_ru.qm

translations.files = $$COMPILEDTRANSLATIONS
translations.path = $$INSTALLDIR/translations
INSTALLS += translations
