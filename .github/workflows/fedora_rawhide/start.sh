#!/bin/bash

set -e

dnf install -y cmake clang ccache ninja-build sqlite-devel libtiff-devel libcurl-devel diffutils

cd "$WORK_DIR"

if test -f "$WORK_DIR/ccache.tar.gz"; then
    echo "Restoring ccache..."
    (cd $HOME && tar xzf "$WORK_DIR/ccache.tar.gz")
fi

export CCACHE_CPP2=yes

ccache -M 500M
ccache -s

mkdir build
cd build
CC=clang CXX=clang++ cmake .. \
    -DEMBED_RESOURCE_FILES=ON -DUSE_ONLY_EMBEDDED_RESOURCE_FILES=ON -DUSE_CCACHE=ON -DPROJ_DB_CACHE_DIR=$HOME/.ccache ..
make -j$(nproc)
ctest -j$(nproc)
cd ..

ccache -s

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && tar czf "$WORK_DIR/ccache.tar.gz" .cache)
