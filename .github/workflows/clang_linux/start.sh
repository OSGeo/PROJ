#!/bin/sh

set -e

apt-get update -y
DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    sudo autoconf automake libtool clang++-10 python3-clang-10 make cmake ccache pkg-config tar zip \
    sqlite3 libsqlite3-dev libtiff-dev libcurl4-openssl-dev jq python3-pip

pip3 install --user jsonschema

cd "$WORK_DIR"

if test -f "$WORK_DIR/ccache.tar.gz"; then
    echo "Restoring ccache..."
    (cd $HOME && tar xzf "$WORK_DIR/ccache.tar.gz")
fi

export CCACHE_CPP2=yes
export PROJ_DB_CACHE_DIR="$HOME/.ccache"

ccache -M 500M
ccache -s

# -fno-use-cxa-atexit is needed to build with -coverage
CC="ccache clang-10" CXX="ccache clang++-10" CFLAGS="-Werror -fsanitize=address -fno-use-cxa-atexit" CXXFLAGS="-Werror -fsanitize=address -fno-use-cxa-atexit" LDFLAGS="-fsanitize=address" ./travis/install.sh

ccache -s

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && tar czf "$WORK_DIR/ccache.tar.gz" .ccache)
