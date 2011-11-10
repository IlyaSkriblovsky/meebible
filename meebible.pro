TEMPLATE = subdirs

SUBDIRS = src share qml translations

TRANSLATIONS += translations/meebible_ru.ts


free:desktop.files = meebible-free.desktop
!free:desktop.files = meebible.desktop
desktop.path = /usr/share/applications
INSTALLS += desktop

free:icon.files = meebible-free.png
!free:icon.files = meebible.png
icon.path = /usr/share/icons/hicolor/64x64/apps
INSTALLS += icon

free:invoker.files = meebible-invoker-free.sh
free:invoker.path = /opt/meebible-free/bin
!free:invoker.files = meebible-invoker.sh
!free:invoker.path = /opt/meebible/bin
INSTALLS += invoker
