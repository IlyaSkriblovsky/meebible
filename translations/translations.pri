TRANSLATIONS += \
    translations/meebible_en.ts \
    translations/meebible_ru.ts

COMPILEDTRANSLATIONS = \
    translations/meebible_en.qm \
    translations/meebible_ru.qm

translations.files = $$COMPILEDTRANSLATIONS
translations.path = $$INSTALLDIR/translations
INSTALLS += translations
