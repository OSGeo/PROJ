#!/bin/bash

set -e

export PROJ_DB_CACHE_DIR="$HOME/.ccache"

ccache -M 200M

CC="clang" CXX="clang++" CFLAGS="-Werror" CXXFLAGS="-Werror" CMAKE_BUILD_TYPE=RelWithDebInfo ./travis/install.sh
