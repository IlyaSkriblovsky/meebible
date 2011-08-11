TEMPLATE = subdirs

SHAREFILES = \
    langs.sqlite    \
    nwt.sqlite      \
    bo.sqlite       \
    nwt.xslt        \
    script.js       \
    style.css       \
    splash.png      \
    splash-ls.png

shares.files = $$SHAREFILES
shares.path = /opt/meebible/share

INSTALLS += shares
