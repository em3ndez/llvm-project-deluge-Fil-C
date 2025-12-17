#!/bin/bash

set -e
set -x

ulimit -c unlimited

cd /sources

tar -xf sqlite-autoconf-3460100.tar.gz
cd sqlite-autoconf-3460100
./configure --prefix=/usr     \
            --disable-static  \
            --enable-fts{4,5} \
            CPPFLAGS="-D SQLITE_ENABLE_COLUMN_METADATA=1 \
                      -D SQLITE_ENABLE_UNLOCK_NOTIFY=1   \
                      -D SQLITE_ENABLE_DBSTAT_VTAB=1     \
                      -D SQLITE_SECURE_DELETE=1"
make
make install
cd ..
rm -rf sqlite-autoconf-3460100
hash -r

tar -xf pizlonated-nettle.tar.gz
cd pizlonated-nettle
./configure --prefix=/usr --disable-static --disable-assembler
make
make install
cd ..
rm -rf pizlonated-nettle
hash -r

tar -xf pizlonated-gnutls.tar.gz
cd pizlonated-gnutls
./configure --prefix=/usr \
            --docdir=/usr/share/doc/gnutls-3.8.7.1 \
            --with-default-trust-store-pkcs11="pkcs11:" \
            --disable-hardware-acceleration
make
make install
cd ..
rm -rf pizlonated-gnutls
hash -r

tar -xf gsettings-desktop-schemas-46.1.tar.xz
cd gsettings-desktop-schemas-46.1
sed -i -r 's:"(/system):"/org/gnome\1:g' schemas/*.in
mkdir -v build
cd build
meson setup --prefix=/usr --buildtype=debugoptimized ..
ninja
ninja install
cd ../..
rm -rf gsettings-desktop-schemas-46.1
hash -r

tar -xf pizlonated-glib-networking.tar.gz
cd pizlonated-glib-networking
mkdir -v build
cd build
meson setup .. --prefix=/usr --buildtype=debugoptimized -D libproxy=disabled
ninja
ninja install
cd ../..
rm -rf pizlonated-glib-networking
hash -r

