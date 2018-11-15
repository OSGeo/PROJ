#!/bin/bash

set -e

./travis/before_install.sh

sudo apt-get update -qq
sudo apt-get install -qq sqlite3 libsqlite3-dev

wget http://releases.llvm.org/6.0.0/clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz
tar xJf clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04.tar.xz
