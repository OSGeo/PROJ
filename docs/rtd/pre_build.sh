#!/bin/sh
set -e

mkdir build
cd build

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_C_FLAGS=-Wno-error -DCMAKE_INSTALL_PREFIX=$HOME/.local ..

make -j install
