#!/bin/bash

set -e

export TRAVIS_OS_NAME=linux
export BUILD_NAME=linux_gcc_32bit
export TRAVIS_BUILD_DIR="$WORK_DIR"

ARCH=i386

dpkg --add-architecture i386
apt update -y

DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends  -o APT::Immediate-Configure=0  \
    autoconf automake libtool gcc-multilib g++-multilib g++ sqlite3 \
    python3-pip \
    make cmake ccache pkg-config tar zip \
    libsqlite3-dev:$ARCH libtiff-dev:$ARCH libcurl4-openssl-dev:$ARCH \
    jq

python3 -m pip install --user jsonschema
export PATH=$HOME/.local/bin:$PATH

export CXXFLAGS='-m32 -D_GLIBCXX_ASSERTIONS'
export CFLAGS='-m32'
export TIFF_CFLAGS=-I/usr/include/i386-linux-gnu
export TIFF_LIBS="-L/usr/lib/i386-linux-gnu -ltiff"
export SQLITE3_CFLAGS=-I/usr/include/i386-linux-gnu
export SQLITE3_LIBS="-L/usr/lib/i386-linux-gnu -lsqlite3"

cd "$WORK_DIR"

if test -f "$WORK_DIR/ccache.tar.gz"; then
    echo "Restoring ccache..."
    (cd $HOME && tar xzf "$WORK_DIR/ccache.tar.gz")
fi

export PROJ_DB_CACHE_DIR="$HOME/.ccache"

ccache -M 500M

CFLAGS="-Werror $CFLAGS" CXXFLAGS="-Werror $CXXFLAGS" CMAKE_BUILD_TYPE=RelWithDebInfo ./travis/install.sh

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && tar czf "$WORK_DIR/ccache.tar.gz" .ccache)
