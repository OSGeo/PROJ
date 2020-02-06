#!/bin/bash

set -e

./travis/before_install_apt.sh
./travis/before_install_pip.sh

sudo apt-get install -qq sqlite3 libsqlite3-dev libtiff-dev libcurl4-openssl-dev

CLANG_LLVM=clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-16.04
wget http://releases.llvm.org/9.0.0/$CLANG_LLVM.tar.xz
tar xJf $CLANG_LLVM.tar.xz
mv $CLANG_LLVM clang+llvm-9
