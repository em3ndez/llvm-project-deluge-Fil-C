#!/bin/bash

set -e
set -x

./build_recover_postlc3.sh
./build_postlc4_continuation.sh

echo OK
