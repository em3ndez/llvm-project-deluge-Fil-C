#!/bin/bash

set -e
set -x

rm -rf pizlonated-gdk-pixbuf
tar -xf pizlonated-gdk-pixbuf.tar.gz
cd pizlonated-gdk-pixbuf
mkdir -v build
cd build
meson setup .. --prefix=/usr --buildtype=debugoptimized -D others=enabled --wrap-mode=nofallback -D man=false
ninja
ninja install
cd ../..
rm -rf pizlonated-gdk-pixbuf
