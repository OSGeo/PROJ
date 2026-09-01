#!/bin/sh

set -e

CLANG_VERSION=20

apt-get update -y
DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    autoconf automake libtool make cmake ccache pkg-config sqlite3 tar zip curl ca-certificates jq \
    python3-pytest python3-ruamel.yaml python3-jsonschema \
    python3-clang-$CLANG_VERSION clang-$CLANG_VERSION libclang-rt-$CLANG_VERSION-dev \
    libsqlite3-dev libgtest-dev \
    libtiff-dev libwebp-dev libzstd-dev \
    libcurl4-openssl-dev libnghttp2-dev libidn2-dev librtmp-dev libssh-dev \
    libpsl-dev libssl-dev libkrb5-dev comerr-dev libldap2-dev libbrotli-dev \
    nlohmann-json3-dev libgtest-dev

cd "$WORK_DIR"

if test -f "$WORK_DIR/ccache.tar.gz"; then
    echo "Restoring ccache..."
    (cd $HOME && tar xzf "$WORK_DIR/ccache.tar.gz")
else
    mkdir -p $HOME/.ccache
fi

ccache -M 500M

# -fno-use-cxa-atexit is needed to build with -coverage
CC="clang-$CLANG_VERSION" CXX="clang++-$CLANG_VERSION" CFLAGS="-fsanitize=address -fno-use-cxa-atexit" CXXFLAGS="-fsanitize=address -fno-use-cxa-atexit" LDFLAGS="-fsanitize=address" ./ci/install.sh

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && tar czf "$WORK_DIR/ccache.tar.gz" .ccache)
