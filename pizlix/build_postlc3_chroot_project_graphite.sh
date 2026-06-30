#!/bin/bash

set -e
set -x

rm -rf graphite2-1.3.14
tar -xf graphite2-1.3.14.tgz
cd graphite2-1.3.14
mkdir -v build
cd build
cmake -D CMAKE_INSTALL_PREFIX=/usr ..
make
make install
cd ../..
rm -rf graphite2-1.3.14
