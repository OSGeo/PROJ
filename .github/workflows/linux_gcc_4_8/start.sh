#!/bin/bash

set -e

export TRAVIS_OS_NAME=linux
export BUILD_NAME=linux_gcc
export TRAVIS_BUILD_DIR="$WORK_DIR"

apt update -y

DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    autoconf automake libtool g++-4.8 sqlite3 \
    python3-pip python3-setuptools \
    make cmake ccache pkg-config tar zip \
    libsqlite3-dev libtiff-dev libcurl4-openssl-dev \
    jq lcov

pip3 install --user --upgrade pip
echo `pip3 --version`
pip3 config --user set global.progress_bar off
pip3 install --user jsonschema
pip3 install --user cmake==3.9.6

export PATH=$HOME/.local/bin:$PATH

export CC="ccache gcc-4.8"
export CXX="ccache g++-4.8"

NPROC=$(nproc)
echo "NPROC=${NPROC}"
export MAKEFLAGS="-j ${NPROC}"

cd "$WORK_DIR"

if test -f "$WORK_DIR/ccache.tar.gz"; then
    echo "Restoring ccache..."
    (cd $HOME && tar xzf "$WORK_DIR/ccache.tar.gz")
fi

export CCACHE_CPP2=yes
export PROJ_DB_CACHE_DIR="$HOME/.ccache"

ccache -M 500M
ccache -s

CFLAGS="-Werror $CFLAGS" CXXFLAGS="-Werror $CXXFLAGS" ./travis/install.sh

ccache -s

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && tar czf "$WORK_DIR/ccache.tar.gz" .ccache)

mkdir -p coverage
lcov --no-external --capture --directory src --output-file coverage/lcov.info
