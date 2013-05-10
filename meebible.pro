VERSION = 3.5.3

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


symbian {
    TARGET.CAPABILITY += NetworkServices
    dev {
        TARGET.UID3 = 0xe0cd6015
    }
    !dev {
        TARGET.UID3 = 0x2005E1E7
        TARGET.CAPABILITY += SwEvent
    }

    DEPLOYMENT.display_name = MeeBible

    ICON = meebible.svg

    vendorinfo = \
        "; Localised Vendor name" \
        "%{\"Ilya Skriblovsky\"}" \
        " " \
        "; Unique Vendor name" \
        ":\"Ilya Skriblovsky\"" \
        " "

    meebible.pkg_prerules += vendorinfo
    DEPLOYMENT += meebible


    qtcomponents11.pkg_prerules = "(0x200346DE), 1, 1, 0, {\"Qt Quick Components for Symbian\"}"
    DEPLOYMENT += qtcomponents11


    include(deployment.pri)
    qtcAddDeployment()
}

iapdonation {
    iapFiles.sources = IAP_VARIANTID.TXT
    iapFiles.path = .
    DEPLOYMENT += iapFiles
}
