#!/bin/bash

set -e
set -x

./build_recover_postlc4.sh
./build_postlc5_continuation.sh

echo OK
