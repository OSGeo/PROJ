#!/bin/sh
set -e

mkdir build
cd build

cmake -GNinja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX=$HOME/.local ..

ninja install
