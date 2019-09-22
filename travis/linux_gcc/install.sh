#!/bin/bash

set -e

export CCACHE_CPP2=yes
export PROJ_DB_CACHE_DIR="$HOME/.ccache"

CC="ccache gcc" CXX="ccache g++" CFLAGS="-std=c99 -Werror" CXXFLAGS="-Werror" ./travis/install.sh
