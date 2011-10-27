TEMPLATE = subdirs

COMPILEDTRANSLATIONS = meebible_ru.qm

translations.files = $$COMPILEDTRANSLATIONS
translations.path = /opt/meebible/translations

INSTALLS += translations
