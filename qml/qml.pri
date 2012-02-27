symbian {
    qml.sources = qml/*.qml qml/*.png
    qml.path = qml
    DEPLOYMENT += qml

    unicomponents.sources = qml/unicomponents/symbian/*.qml qml/unicomponents/symbian/*.png
    unicomponents.path = qml/unicomponents
    DEPLOYMENT += unicomponents
} else {
    qml.files = qml/*.qml qml/*.png
    qml.path = $$INSTALLDIR/qml
    INSTALLS += qml

    unicomponents.files = qml/unicomponents/meego/*.qml
    unicomponents.path = $$INSTALLDIR/qml/unicomponents
    INSTALLS += unicomponents
}
