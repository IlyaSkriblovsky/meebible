TEMPLATE = subdirs

qml.files = *.qml *.png

free:qml.path = /opt/meebible-free/qml
!free:qml.path = /opt/meebible/qml
INSTALLS += qml
