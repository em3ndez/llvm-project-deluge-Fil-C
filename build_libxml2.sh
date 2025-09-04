#!/bin/sh

. libpas/common.sh

set -e
set -x

cd projects/libxml2-2.14.4
extract_source
CC=$PWD/../../../build/bin/clang PATH=$PWD/../../../pizfix/bin:$PATH ./configure --prefix=$PWD/../../../pizfix --disable-maintainer-mode --with-icu
make -j $NCPU
make check -j $NCPU
make install -j $NCPU
