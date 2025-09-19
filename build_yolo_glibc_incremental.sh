#!/bin/sh

. libpas/common.sh

set -e
set -x

cd pizlonated-yolo-glibc-build
make -j $NCPU
make -j $NCPU install

../fix_yolo_glibc.sh
