#!/bin/sh
set -e

mkdir build
cd build

cmake -DCMAKE_BUILD_TYPE=Release -DPROJ_TESTS=OFF -DCMAKE_INSTALL_PREFIX=$HOME/.local ..

make -j install
