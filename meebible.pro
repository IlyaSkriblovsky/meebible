TEMPLATE = subdirs

SUBDIRS = src share qml


desktop.files = meebible.desktop
desktop.path = /usr/share/applications

icon.files = meebible.png
icon.path = /usr/share/icons/hicolor/64x64/apps

invoker.files = meebible-invoker.sh
invoker.path = /opt/meebible/bin

INSTALLS += desktop icon invoker
