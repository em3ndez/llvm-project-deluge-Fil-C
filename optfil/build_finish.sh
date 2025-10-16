#!/bin/sh
#
# Copyright (c) 2025 Epic Games, Inc. All Rights Reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions
# are met:
# 1. Redistributions of source code must retain the above copyright
#    notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright
#    notice, this list of conditions and the following disclaimer in the
#    documentation and/or other materials provided with the distribution.
#
# THIS SOFTWARE IS PROVIDED BY EPIC GAMES, INC. ``AS IS AND ANY
# EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
# PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL EPIC GAMES, INC. OR
# CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
# OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. 

set -e
set -x

ulimit -c unlimited

test $EUID -ne 0

test "x$FILCSRC" != "x"
test -d $FILCSRC
test -d $FILCSRC/libpas
test -d $FILCSRC/llvm
test -d $FILCSRC/clang
test -d $FILCSRC/filc
test -d $FILCSRC/optfil

cd $FILCSRC/optfil

. ../libpas/common.sh

package_name=optfil-0.673-$OS-$ARCH

rm -rf $package_name
mkdir -v $package_name

cp -v /opt/fil.tar $package_name/
cp -v ../README.md $package_name/
cp ../LLVM-LICENSE.txt $package_name/
cp ../libpas/LICENSE.txt $package_name/PAS-LICENSE.txt
cp -v setup.sh $package_name/

# Copy all project license files
cp bash-LICENSE.txt $package_name/
cp bzip2-LICENSE.txt $package_name/
cp coreutils-LICENSE.txt $package_name/
cp glibc-LICENSE.txt $package_name/
cp libffi-LICENSE.txt $package_name/
cp libxcrypt-LICENSE.txt $package_name/
cp lz4-LICENSE.txt $package_name/
cp mg-LICENSE.txt $package_name/
cp ncurses-LICENSE.txt $package_name/
cp openssh-LICENSE.txt $package_name/
cp openssl-LICENSE.txt $package_name/
cp pkgconf-LICENSE.txt $package_name/
cp readline-LICENSE.txt $package_name/
cp xz-LICENSE.txt $package_name/
cp zlib-LICENSE.txt $package_name/
cp zstd-LICENSE.txt $package_name/

tar -cJf $package_name.tar.xz $package_name
