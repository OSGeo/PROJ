#!/bin/sh

set -e

apt-get update -y
DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    sudo autoconf automake libtool clang++-10 python3-clang-10 make cmake ccache pkg-config tar zip \
    sqlite3 libsqlite3-dev libtiff-dev libcurl4-openssl-dev jq python3-pip nlohmann-json3-dev

python3 -m pip install --user jsonschema
export PATH=$HOME/.local/bin:$PATH

cd "$WORK_DIR"

if test -f "$WORK_DIR/ccache.tar.gz"; then
    echo "Restoring ccache..."
    (cd $HOME && tar xzf "$WORK_DIR/ccache.tar.gz")
fi

export PROJ_DB_CACHE_DIR="$HOME/.ccache"

ccache -M 500M

# -fno-use-cxa-atexit is needed to build with -coverage
CC="clang-10" CXX="clang++-10" CFLAGS="-Werror -fsanitize=address -fno-use-cxa-atexit" CXXFLAGS="-Werror -fsanitize=address -fno-use-cxa-atexit" LDFLAGS="-fsanitize=address" ./travis/install.sh

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && tar czf "$WORK_DIR/ccache.tar.gz" .ccache)
