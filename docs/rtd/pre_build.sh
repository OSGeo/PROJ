#!/bin/sh
set -e

cmake \
    -G Ninja \
    -D CMAKE_BUILD_TYPE=Release \
    -D BUILD_TESTING=OFF \
    -D CMAKE_INSTALL_PREFIX=$HOME/.local \
    -S . -B build

cmake --build build

cmake --install build
