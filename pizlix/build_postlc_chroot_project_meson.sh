#!/bin/bash

set -e
set -x

rm -rf pizlonated-meson
tar -xf pizlonated-meson.tar.gz
cd pizlonated-meson
pip3 wheel -w dist --no-cache-dir --no-build-isolation --no-deps $PWD
pip3 install --no-index --find-links dist meson
install -vDm644 data/shell-completions/bash/meson /usr/share/bash-completion/completions/meson
install -vDm644 data/shell-completions/zsh/_meson /usr/share/zsh/site-functions/_meson
cd ..
rm -rf pizlonated-meson

