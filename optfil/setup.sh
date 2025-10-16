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

VERSION="0.673"

echo "================================================================================"
echo "                         Fil-C /opt/fil Distribution"
echo "                              Version $VERSION"
echo "================================================================================"
echo
echo "Fil-C is a memory-safe implementation of C and C++ that prevents all memory"
echo "safety errors through concurrent garbage collection and invisible capabilities."
echo
echo "This distribution includes:"
echo "  - Fil-C compiler (filcc, fil++)"
echo "  - Basic libraries (glibc, C++ standard library)"
echo "  - GNU Bash (memory-safe)"
echo "  - GNU Coreutils (memory-safe)"
echo "  - Compression utilities (bzip2, xz, lz4, zstd)"
echo "  - OpenSSL cryptographic library"
echo "  - OpenSSH client and server"
echo
echo "********************************************************************************"
echo "                                  WARNING!"
echo "********************************************************************************"
echo "This is an EXPERIMENTAL RELEASE version $VERSION"
echo
echo "Fil-C is still under active development. Performance is currently 1.5x-5x"
echo "slower than standard C. This software should NOT be used in production"
echo "environments without thorough testing."
echo "********************************************************************************"
echo
echo "This installer will extract fil.tar to /opt, creating /opt/fil"
echo
echo "Type YES (in all caps) to proceed with installation, or anything else to abort:"
read response

if [ "$response" != "YES" ]; then
    echo "Installation aborted."
    exit 1
fi

echo
echo "Extracting fil.tar to /opt..."

if [ ! -f fil.tar ]; then
    echo "Error: fil.tar not found in current directory"
    exit 1
fi

cd /opt
tar -xf "$OLDPWD/fil.tar"

echo
echo "================================================================================"
echo "                         Installation Complete!"
echo "================================================================================"
echo
echo "To use Fil-C, add /opt/fil/bin to your PATH:"
echo "  export PATH=/opt/fil/bin:\$PATH"
echo
echo "You can use the memory-safe OpenSSH server at:"
echo "  /opt/fil/sbin/sshd"
echo
echo "To compile C programs with Fil-C:"
echo "  filcc -o program program.c -g -O"
echo
echo "To compile C++ programs with Fil-C:"
echo "  fil++ -o program program.cpp -g -O -std=c++20"
echo
echo "For more information, see README.md in the distribution directory."
echo