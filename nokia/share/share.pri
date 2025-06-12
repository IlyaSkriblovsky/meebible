SHAREFILES = \
    share/script.js                 \
    share/style.css                 \
    share/splash.png                \
    share/splash-ls.png             \
    share/splash-inverted.png       \
    share/splash-ls-inverted.png

symbian {
    shares.sources = $$SHAREFILES
    shares.path = share
    DEPLOYMENT += shares
} else {
    shares.files = $$SHAREFILES
    shares.path = $$INSTALLDIR/share
    INSTALLS += shares
}
