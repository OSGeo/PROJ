#!/bin/bash

set -e

ccache -M 200M

CC="clang" CXX="clang++" CFLAGS="-Werror" CXXFLAGS="-Werror" CMAKE_BUILD_TYPE=RelWithDebInfo ./travis/install.sh
