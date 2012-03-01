# Wildcard doesn't work here
QMAKE_SUBSTITUTES += \
    qml/AboutPage.qml.in            \
    qml/BibleViewPage.qml.in        \
    qml/BookmarkItem.qml.in         \
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
    qml/main.qml.in

symbian {
    QML_IMPORT_QTCOMPONENTS = "import com.nokia.symbian 1.1"

    qml.sources = $$BUILDDIR/qml/*.qml qml/*.png
    qml.path = qml
    DEPLOYMENT += qml

    unicomponents.sources = qml/unicomponents/symbian/*.qml qml/unicomponents/symbian/*.png
    unicomponents.path = qml/unicomponents
    DEPLOYMENT += unicomponents
} else {
    QML_IMPORT_QTCOMPONENTS = "import com.nokia.meego 1.0"

    qml.files = $$BUILDDIR/qml/*.qml qml/*.png
    qml.path = $$INSTALLDIR/qml
    INSTALLS += qml

    unicomponents.files = qml/unicomponents/meego/*.qml
    unicomponents.path = $$INSTALLDIR/qml/unicomponents
    INSTALLS += unicomponents
}
