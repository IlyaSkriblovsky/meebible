if grep inverted=true /home/user/.config/MeeBible/MeeBible.conf ; then
    /usr/bin/invoker -s --type=d --splash=/opt/meebible-free/share/splash-inverted.png --splash-landscape=/opt/meebible-free/share/splash-ls-inverted.png /opt/meebible-free/bin/meebible
else
    /usr/bin/invoker -s --type=d --splash=/opt/meebible-free/share/splash.png --splash-landscape=/opt/meebible-free/share/splash-ls.png /opt/meebible-free/bin/meebible
fi
