#!/bin/bash
#
# Copyright (c) 2026 Epic Games, Inc. All Rights Reserved.
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

test $EUID -eq 0
test -d ..
test -d ../projects
test -d ../llvm
test -d etc
test -e LFS-12.2-SYSV-BOOK.pdf

CLEANUP=""
defer() {
    CLEANUP="$1; $CLEANUP"
    trap "set +e; $CLEANUP" EXIT
}

test ! -e data-disk.img

truncate -s 100G data-disk.img

parted data-disk.img <<EOF
mklabel gpt
mkpart data 1MiB 100%
EOF

LOOP=$(losetup -Pf --show data-disk.img)
defer "losetup -d $LOOP"
udevadm settle

ls $LOOP*
test -e $LOOP
test -e ${LOOP}p1

mkfs.ext4 -L root ${LOOP}p1

cat > data-disk.vmdk <<EOF
# Disk DescriptorFile
version=1
CID=fffffffe
parentCID=ffffffff
createType="monolithicFlat"

RW $(($(stat -c%s data-disk.img) / 512)) FLAT "data-disk.img" 0
EOF

FILCOWNER=`stat -c %U ..`
FILCGROUP=`stat -c %G ..`
id -u $FILCOWNER
getent group $FILCGROUP
chown $FILCOWNER:$FILCGROUP data-disk.img data-disk.vmdk

