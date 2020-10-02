#!/bin/bash

set -e

export CCACHE_CPP2=yes
export PROJ_DB_CACHE_DIR="$HOME/.ccache"

# -fno-use-cxa-atexit is needed to build with -coverage
CC="ccache $CC" CXX="ccache $CXX" CFLAGS="-Werror" CXXFLAGS="-Werror" ./travis/install.sh
