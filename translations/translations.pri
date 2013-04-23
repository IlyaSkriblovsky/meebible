TRANSLATIONS += \
    translations/meebible_en.ts \
    translations/meebible_ru.ts \
    translations/meebible_es.ts \
    translations/meebible_it.ts \
    translations/meebible_sv.ts \
    translations/meebible_pt.ts \
    translations/meebible_fi.ts \
    translations/meebible_de.ts


COMPILEDTRANSLATIONS = translations/*.qm


symbian {
    translations.sources = $$COMPILEDTRANSLATIONS
    translations.path = translations
    DEPLOYMENT += translations
} else {
    translations.files = $$COMPILEDTRANSLATIONS
    translations.path = $$INSTALLDIR/translations
    INSTALLS += translations
}



# Auto-compiling translations

isEmpty(QMAKE_LRELEASE) {
    QMAKE_LRELEASE = $$[QT_INSTALL_BINS]/lrelease
}

TS_DIR = translations
TSQM.name = lrelease ${QMAKE_FILE_IN}
TSQM.input = TRANSLATIONS
TSQM.output = $$TS_DIR/${QMAKE_FILE_BASE}.qm
TSQM.commands = $$QMAKE_LRELEASE ${QMAKE_FILE_IN}
TSQM.CONFIG = no_link
QMAKE_EXTRA_COMPILERS += TSQM
PRE_TARGETDEPS += compiler_TSQM_make_all
