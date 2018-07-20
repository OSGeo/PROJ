#!/bin/bash

set -e

export CCACHE_CPP2=yes

CC="ccache clang" CXX="ccache clang++" CFLAGS="-Werror -O2" CXXFLAGS="-Werror -O2" ./travis/install.sh
