#!/bin/bash

set -e
set -x

ulimit -c unlimited

test $EUID -ne 0

test "x$FILCSRC" != "x"
test -d $FILCSRC
test -d $FILCSRC/projects

cd $FILCSRC

rm -vf projects/*/pizlonated-*.tar.gz
./package-source.sh projects/nettle-3.10 pizlonated-nettle
./package-source.sh projects/gnutls-3.8.7.1 pizlonated-gnutls


