#!/bin/bash

set -e

export CCACHE_CPP2=yes

# -fno-use-cxa-atexit is needed to build with -coverage
CC="ccache clang" CXX="ccache clang++" CFLAGS="-Werror -fsanitize=address -fno-use-cxa-atexit" CXXFLAGS="-Werror -fsanitize=address -fno-use-cxa-atexit" LDFLAGS="-fsanitize=address" ./travis/install.sh
