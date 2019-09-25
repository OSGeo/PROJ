#!/bin/bash

set -e

./travis/before_install_apt.sh
./travis/before_install_pip.sh

sudo apt-get install -qq \
        wget \
        binutils-mingw-w64-x86-64 \
        gcc-mingw-w64-x86-64 \
        g++-mingw-w64-x86-64 \
        mingw-w64-tools \
        wine1.4-amd64 \
        sqlite3
