TEMPLATE = subdirs

COMPILEDTRANSLATIONS = meebible_ru.qm

translations.files = $$COMPILEDTRANSLATIONS
free:translations.path = /opt/meebible-free/translations
!free:translations.path = /opt/meebible/translations
INSTALLS += translations
