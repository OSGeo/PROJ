#!/bin/bash

set -e

export TRAVIS_OS_NAME=linux
export BUILD_NAME=linux_gcc
export TRAVIS_BUILD_DIR="$WORK_DIR"

apt-get update -y

DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    autoconf automake libtool make cmake ccache pkg-config python3-pip sqlite3 tar zip \
    g++ jq lcov python3-setuptools \
    libsqlite3-dev \
    libtiff-dev \
    libcurl4-openssl-dev libidn11-dev librtmp-dev libssl-dev libkrb5-dev comerr-dev libldap2-dev

python3 -m pip install --user --upgrade "pip < 21.0"
echo `python3 -m pip --version`
python3 -m pip config --user set global.progress_bar off
python3 -m pip install --user jsonschema
python3 -m pip install --user cmake==3.9.6

export PATH=$HOME/.local/bin:$PATH

cd "$WORK_DIR"

if test -f "$WORK_DIR/ccache.tar.gz"; then
    echo "Restoring ccache..."
    (cd $HOME && tar xzf "$WORK_DIR/ccache.tar.gz")
fi

export PROJ_DB_CACHE_DIR="$HOME/.ccache"

ccache -M 500M

CFLAGS="-Werror $CFLAGS" CXXFLAGS="-Werror $CXXFLAGS" ./travis/install.sh

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && tar czf "$WORK_DIR/ccache.tar.gz" .ccache)

mkdir -p coverage
lcov --no-external --capture --directory src --output-file coverage/lcov.info
