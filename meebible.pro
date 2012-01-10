TEMPLATE = app

!free:INSTALLDIR = /opt/meebible
free: INSTALLDIR = /opt/meebible-free


include(src/src.pri)
include(share/share.pri)
include(qml/qml.pri)
include(translations/translations.pri)


free:desktop.files = meebible-free.desktop
!free:desktop.files = meebible.desktop
desktop.path = /usr/share/applications
INSTALLS += desktop

free:icon.files = meebible-free.png
!free:icon.files = meebible.png
icon.path = /usr/share/icons/hicolor/64x64/apps
INSTALLS += icon

free:invoker.files = meebible-invoker-free.sh
!free:invoker.files = meebible-invoker.sh
invoker.path = $$INSTALLDIR/bin
INSTALLS += invoker
