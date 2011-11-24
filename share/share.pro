TEMPLATE = subdirs

SHAREFILES = \
    langs.sqlite    \
    nwt.sqlite      \
    bo.sqlite       \
    blv.sqlite      \
    nwt.xslt        \
    script.js       \
    style.css       \
    splash.png      \
    splash-ls.png   \
    splash-inverted.png     \
    splash-ls-inverted.png

shares.files = $$SHAREFILES

free:shares.path = /opt/meebible-free/share
!free:shares.path = /opt/meebible/share

INSTALLS += shares
