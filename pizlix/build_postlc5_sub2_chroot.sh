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

