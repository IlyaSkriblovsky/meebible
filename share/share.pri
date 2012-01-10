SHAREFILES = \
    share/langs.sqlite              \
    share/nwt.sqlite                \
    share/bo.sqlite                 \
    share/blv.sqlite                \
    share/ccarabic.sqlite           \
    share/kjbo.sqlite               \
    share/nwt.xslt                  \
    share/script.js                 \
    share/style.css                 \
    share/splash.png                \
    share/splash-ls.png             \
    share/splash-inverted.png       \
    share/splash-ls-inverted.png

shares.files = $$SHAREFILES
shares.path = $$INSTALLDIR/share
INSTALLS += shares
