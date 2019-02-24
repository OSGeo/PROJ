#!/bin/bash

set -e

export CCACHE_CPP2=yes

CC="ccache $CC" CXX="ccache $CXX" CFLAGS="-std=c89 -Werror $CFLAGS" CXXFLAGS="-Werror $CXXFLAGS" ./travis/install.sh
