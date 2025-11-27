#!/bin/bash

set -e
set -x

ulimit -c unlimited

cd /sources

tar -xf pizlonated-yaml.tar.gz
cd pizlonated-yaml
./configure --enable-shared --prefix=/usr
make
make install
cd ..
rm -rf pizlonated-yaml
hash -r

tar -xf pizlonated-ruby.tar.gz
cd pizlonated-ruby
./configure --enable-shared --disable-yjit --disable-rjit --without-jit --without-gcc --without-valgrind --with-thread=pthread --without-jemalloc --with-coroutine=pthread --prefix=/usr
make
make install
cd ..
rm -rf pizlonated-ruby
hash -r

tar -xf fribidi-1.0.15.tar.xz
cd fribidi-1.0.15
mkdir -v build
cd build
meson setup --prefix=/usr --buildtype=debugoptimized
ninja
ninja install
cd ../..
rm -rf fribidi-1.0.15
hash -r
