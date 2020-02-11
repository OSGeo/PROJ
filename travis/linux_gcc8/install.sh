#!/bin/bash

set -e

export CCACHE_CPP2=yes
export PROJ_DB_CACHE_DIR="$HOME/.ccache"

CC="ccache $CC" CXX="ccache $CXX" CFLAGS="-std=c99 -Werror $CFLAGS" CXXFLAGS="-Werror $CXXFLAGS" ./travis/install.sh
