#!/bin/sh

set -e

apt-get update -y
DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    autoconf automake libtool make cmake ccache pkg-config python3-pip sqlite3 tar zip \
    clang++-10 jq python3-clang-10 \
    libsqlite3-dev \
    libtiff-dev libwebp-dev libzstd-dev \
    libcurl4-openssl-dev libnghttp2-dev libidn2-dev librtmp-dev libssh-dev \
      libpsl-dev libssl-dev libkrb5-dev comerr-dev libldap2-dev libbrotli-dev \
    nlohmann-json3-dev libgtest-dev

python3 -m pip install --user jsonschema
export PATH=$HOME/.local/bin:$PATH

cd "$WORK_DIR"

if test -f "$WORK_DIR/ccache.tar.gz"; then
    echo "Restoring ccache..."
    (cd $HOME && tar xzf "$WORK_DIR/ccache.tar.gz")
fi

ccache -M 500M

# -fno-use-cxa-atexit is needed to build with -coverage
CC="clang-10" CXX="clang++-10" CFLAGS="-Werror -fsanitize=address -fno-use-cxa-atexit" CXXFLAGS="-Werror -fsanitize=address -fno-use-cxa-atexit" LDFLAGS="-fsanitize=address" ./travis/install.sh

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && tar czf "$WORK_DIR/ccache.tar.gz" .ccache)
