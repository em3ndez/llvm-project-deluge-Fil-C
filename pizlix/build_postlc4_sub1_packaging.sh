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
./package-source.sh projects/pygobject-3.48.2 pizlonated-pygobject
./package-source.sh projects/graphene-1.10.8 pizlonated-graphene
./package-source.sh projects/mesa-24.1.5 pizlonated-mesa

