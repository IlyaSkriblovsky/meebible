#!/bin/sh
make clean distclean
rm -rf debian/meebible debian/meebible.substvars debian/meebible.debhelper.log debian/files builddir build-stamp
rm -rf debian/meebible-free debian/meebible-free.substvars debian/meebible-free.debhelper.log debian/files builddir-free build-stamp-free
rm -rf ../meebible_* ../meebible-free_*
rm -rf translations/*.qm
rm -rf clean
