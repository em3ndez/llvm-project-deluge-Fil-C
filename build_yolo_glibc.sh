#!/bin/sh

. libpas/common.sh

set -e
set -x

cd projects/yolo-glibc-2.40
rm -f configure
autoconf
cd ../..
rm -rf pizlonated-yolo-glibc-build
mkdir pizlonated-yolo-glibc-build
(cd pizlonated-yolo-glibc-build && ../projects/yolo-glibc-2.40/configure --prefix=$PWD/../pizfix/yolo --disable-mathvec)
./build_yolo_glibc_incremental.sh
