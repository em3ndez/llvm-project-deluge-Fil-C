#!/bin/bash

set -e
set -x

mkdir build
cd build
cmake .. -DPORT=JSCOnly -DCMAKE_BUILD_TYPE=Release -G Ninja -DENABLE_JIT=OFF -DENABLE_C_LOOP=ON -DENABLE_SAMPLING_PROFILER=OFF -DENABLE_WEBASSEMBLY=OFF

set +x

echo "SUCCESS"
echo "Now do (cd build && ninja) to build."
