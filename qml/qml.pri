# Wildcard doesn't work here
QMAKE_SUBSTITUTES += \
    qml/main.qml.in                 \
    qml/AboutPage.qml.in            \
    qml/BibleViewPage.qml.in        \
    qml/ListItem.qml.in             \
    qml/BookmarksSheet.qml.in       \
    qml/ButtonWithHelp.qml.in       \
    qml/FeedbackPage.qml.in         \
    qml/FetcherDialog.qml.in        \
    qml/Header.qml.in               \
    qml/HelpButton.qml.in           \
    qml/LabeledSwitch.qml.in        \
    qml/LanguageDialog.qml.in       \
    qml/ModelSelectionDialog.qml.in \
    qml/NumberSelector.qml.in       \
    qml/PlaceDialog.qml.in          \
    qml/SearchDialog.qml.in         \
    qml/SettingsPage.qml.in         \
    qml/SimpleListDelegate.qml.in   \
    qml/TranslationDialog.qml.in    \
    qml/TumblerItem.qml.in          \
    qml/SubtitledListDelegate.qml.in\
    qml/NumberGridSelector.qml.in   \
    qml/BookGridDelegate.qml.in     \
    qml/GridPlaceDialog.qml.in

symbian {
    QML_IMPORT_QTCOMPONENTS = "import com.nokia.symbian 1.1"
    QML_IMPORT_QTCOMPONENTS_EXTRAS = "import com.nokia.extras 1.1"
    QML_SYMBIAN_THEME = "platformInverted: !settings.inverted"

    qml.sources = qml/*.qml qml/*.js qml/*.png
    qml.path = qml
    DEPLOYMENT += qml

    unicomponents.sources = qml/unicomponents/symbian/*.qml qml/unicomponents/symbian/*.png
    unicomponents.path = qml/unicomponents
    DEPLOYMENT += unicomponents
} else {
    QML_IMPORT_QTCOMPONENTS = "import com.meego 1.0"
    QML_IMPORT_QTCOMPONENTS_EXTRAS = "import com.meego.extras 1.0"

    qml.files = $$BUILDDIR/qml/*.qml qml/*.js qml/*.png
    qml.path = $$INSTALLDIR/qml
    INSTALLS += qml

    unicomponents.files = qml/unicomponents/meego/*.qml
    unicomponents.path = $$INSTALLDIR/qml/unicomponents
    INSTALLS += unicomponents
}
