#!/bin/bash

set -e

export CCACHE_CPP2=yes

CC="ccache gcc" CXX="ccache g++" CFLAGS="-std=c89 -Werror" CXXFLAGS="-Werror" ./travis/install.sh
