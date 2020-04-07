#!/bin/bash

set -e

export CCACHE_CPP2=yes
export PROJ_DB_CACHE_DIR="$HOME/.ccache"

CC="ccache $CC" CXX="ccache $CXX" CFLAGS="-std=c99 -Werror" CXXFLAGS="-Werror" ./travis/install.sh
