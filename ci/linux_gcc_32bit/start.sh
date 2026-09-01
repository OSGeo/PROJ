#!/bin/bash

set -e

export TRAVIS_OS_NAME=linux
export BUILD_NAME=linux_gcc_32bit
export TRAVIS_BUILD_DIR="$WORK_DIR"

GCC_VERSION=12

ARCH=i386

dpkg --add-architecture i386
apt-get update -y

DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends  -o APT::Immediate-Configure=0  \
    autoconf automake libtool make cmake ccache pkg-config sqlite3 tar zip curl ca-certificates jq \
    python3-pytest python3-ruamel.yaml python3-jsonschema \
    gcc-$GCC_VERSION-multilib g++-$GCC_VERSION-multilib g++-$GCC_VERSION \
    libsqlite3-dev:$ARCH libgtest-dev:$ARCH \
    libtiff-dev:$ARCH libwebp-dev:$ARCH libzstd-dev:$ARCH \
    libcurl4-openssl-dev:$ARCH libnghttp2-dev:$ARCH libidn2-dev:$ARCH librtmp-dev:$ARCH libssh-dev:$ARCH \
    libpsl-dev:$ARCH libssl-dev:$ARCH libkrb5-dev:$ARCH comerr-dev:$ARCH libldap2-dev:$ARCH libbrotli-dev:$ARCH

LIBDIR=/usr/lib/i386-linux-gnu
INCDIR=/usr/include/i386-linux-gnu

export CXXFLAGS="-m32 -D_GLIBCXX_ASSERTIONS"
export CFLAGS="-m32"
export TIFF_CFLAGS="-I$INCDIR"
export TIFF_LIBS="-L$LIBDIR -ltiff"
export SQLITE3_CFLAGS="-I$INCDIR"
export SQLITE3_LIBS="-L$LIBDIR -lsqlite3"

export PKG_CONFIG=i686-linux-gnu-pkg-config

cd "$WORK_DIR"

if test -f "$WORK_DIR/ccache.tar.gz"; then
    echo "Restoring ccache..."
    (cd $HOME && tar xzf "$WORK_DIR/ccache.tar.gz")
else
    mkdir -p $HOME/.ccache
fi

ccache -M 500M

CC="gcc-$GCC_VERSION" CXX="g++-$GCC_VERSION" CMAKE_BUILD_TYPE=RelWithDebInfo ./ci/install.sh

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && tar czf "$WORK_DIR/ccache.tar.gz" .ccache)
