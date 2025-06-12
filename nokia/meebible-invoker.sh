if grep inverted=true /home/user/.config/MeeBible/MeeBible.conf ; then
    /usr/bin/invoker -s --type=d --splash=/opt/meebible/share/splash-inverted.png --splash-landscape=/opt/meebible/share/splash-ls-inverted.png /opt/meebible/bin/meebible
else
    /usr/bin/invoker -s --type=d --splash=/opt/meebible/share/splash.png --splash-landscape=/opt/meebible/share/splash-ls.png /opt/meebible/bin/meebible
fi
