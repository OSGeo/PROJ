#!/bin/bash

set -e

export PROJ_DB_CACHE_DIR="$HOME/.ccache"

# -fno-use-cxa-atexit is needed to build with -coverage
CFLAGS="-Werror" CXXFLAGS="-Werror" ./travis/install.sh
