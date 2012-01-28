SHAREFILES = \
    # share/trans.sqlite              \
    # share/langs.sqlite              \
    # share/nwt.xslt                  \
    share/script.js                 \
    share/style.css                 \
    share/splash.png                \
    share/splash-ls.png             \
    share/splash-inverted.png       \
    share/splash-ls-inverted.png

shares.files = $$SHAREFILES
shares.path = $$INSTALLDIR/share
INSTALLS += shares
