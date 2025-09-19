#!/bin/sh

if test `basename $PWD` != pizlonated-yolo-glibc-build
then
    echo "Please run this script from the pizlonated-yolo-glibc-build directory."
    exit 1
fi

. ../libpas/common.sh

set -e
set -x

cd ../pizfix

OLDLDNAME=ld-linux-x86-64.so.2
OLDLIBCIMPLNAME=libc.so.6
OLDLIBCNONSHAREDNAME=libc_nonshared.a
OLDLIBMIMPLNAME=libm.so.6
OLDSTATICLIBCNAME=libc.a
OLDSTATICLIBMNAME=libm.a

LDNAME=ld-yolo-x86_64.so
LIBNAMEBASE=libyolo
LIBCNAMEBASE=${LIBNAMEBASE}c
LIBCNAME=${LIBCNAMEBASE}.so
LIBCIMPLNAME=${LIBCNAMEBASE}impl.so
LIBCNONSHAREDNAME=${LIBCNAMEBASE}_nonshared.a
LIBMIMPLNAME=${LIBNAMEBASE}mimpl.so
LIBMNAME=${LIBNAMEBASE}m.so
STATICLIBCNAME=${LIBCNAMEBASE}.a
STATICLIBMNAME=${LIBNAMEBASE}m.a

mkdir -p lib
cp yolo/lib/$OLDLDNAME lib/$LDNAME
cp yolo/lib/$OLDLIBCIMPLNAME lib/$LIBCIMPLNAME
cp yolo/lib/$OLDLIBCNONSHAREDNAME lib/$LIBCNONSHAREDNAME
cp yolo/lib/$OLDLIBMIMPLNAME lib/$LIBMIMPLNAME
cp yolo/lib/*.o lib/
cp yolo/lib/$OLDSTATICLIBCNAME lib/$STATICLIBCNAME
cp yolo/lib/$OLDSTATICLIBMNAME lib/$STATICLIBMNAME
patchelf --replace-needed $OLDLDNAME $LDNAME lib/$LIBCIMPLNAME
patchelf --set-soname $LIBCIMPLNAME lib/$LIBCIMPLNAME
patchelf --set-soname $LDNAME lib/$LDNAME
patchelf --replace-needed $OLDLDNAME $LDNAME lib/$LIBMIMPLNAME
patchelf --replace-needed $OLDLIBCIMPLNAME $LIBCIMPLNAME lib/$LIBMIMPLNAME
patchelf --set-soname $LIBMIMPLNAME lib/$LIBMIMPLNAME
echo "OUTPUT_FORMAT(elf64-x86-64)" > lib/$LIBCNAME
echo "GROUP ( $PWD/lib/$LIBCIMPLNAME $PWD/lib/$LIBCNONSHAREDNAME  AS_NEEDED ( $PWD/lib/$LDNAME ) )" >> lib/$LIBCNAME
echo "OUTPUT_FORMAT(elf64-x86-64)" > lib/$LIBMNAME
echo "GROUP ( $PWD/lib/$LIBMIMPLNAME )" >> lib/$LIBMNAME

rm -rf yolo-include
mv yolo/include yolo-include
rm -rf yolo
