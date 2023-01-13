#!/bin/sh
set -e

echo `pwd`
mkdir build
cd build

CONDA_PREFIX="/home/docs/checkouts/readthedocs.org/user_builds/$READTHEDOCS_PROJECT/conda/$READTHEDOCS_VERSION"

echo "CONDA_PREFIX: $CONDA_PREFIX"


LDFLAGS="-Wl,-rpath-link,$CONDA_PREFIX/lib" cmake -G Ninja -DCMAKE_BUILD_TYPE=Release -DBUILD_TESTING=OFF -DCMAKE_INSTALL_PREFIX="$CONDA_PREFIX" -DCMAKE_PREFIX_PATH="$CONDA_PREFIX" ..

ninja install
