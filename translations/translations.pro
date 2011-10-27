TEMPLATE = subdirs

TRANSLATIONS = meebible_ru.ts

translations.files = $$TRANSLATIONS
translations.path = /opt/meebible/translations

INSTALLS += translations
