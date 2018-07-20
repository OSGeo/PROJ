#!/bin/bash

set -e

export CCACHE_CPP2=yes

CC="ccache clang" CXX="ccache clang++" CFLAGS="-std=c89 -Werror -fsanitize=address" CXXFLAGS="-Werror -fsanitize=address" LDFLAGS="-fsanitize=address" ./travis/install.sh
