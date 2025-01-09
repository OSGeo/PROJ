#!/bin/bash

set -e

dnf install -y cmake clang ccache ninja-build sqlite-devel libtiff-devel libcurl-devel diffutils wget

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
echo "Build with -DEMBED_RESOURCE_FILES=ON"
CC=clang CXX=clang++ cmake .. \
    -DEMBED_RESOURCE_FILES=ON -DUSE_CCACHE=ON -DPROJ_DB_CACHE_DIR=$HOME/.ccache ..
make -j$(nproc)
ctest -j$(nproc)

# Try EMBED_RESOURCE_DIRECTORY option
wget https://raw.githubusercontent.com/OSGeo/PROJ-data/refs/heads/master/us_nga/us_nga_egm96_15.tif
mkdir grids
mv us_nga_egm96_15.tif grids
echo "Build with -DEMBED_RESOURCE_FILES=ON -DEMBED_RESOURCE_DIRECTORY=$PWD/grids"
CC=clang CXX=clang++ cmake .. -DEMBED_RESOURCE_DIRECTORY=$PWD/grids
make -j$(nproc)
rm -rf data
echo 49 2 0 | bin/cs2cs "WGS84 + EGM96 height" EPSG:4979
echo 49 2 0 | bin/cs2cs "WGS84 + EGM96 height" EPSG:4979  | grep 44.643 >/dev/null || (echo "Expected 49dN 2dE 44.643 as a result" && /bin/false)
echo 0 0 0 | bin/cct +init=ITRF2000:ITRF96
echo 0 0 0 | bin/cct +init=ITRF2000:ITRF96 | grep 0.0067 >/dev/null || (echo "Expected 0.0067 0.0061 -0.0185 as a result" && /bin/false)

echo "Build with -DEMBED_RESOURCE_FILES=ON -DEMBED_RESOURCE_DIRECTORY=$PWD/grids -DUSE_ONLY_EMBEDDED_RESOURCE_FILES=ON"
CC=clang CXX=clang++ cmake .. -DEMBED_RESOURCE_DIRECTORY=$PWD/grids -DUSE_ONLY_EMBEDDED_RESOURCE_FILES=ON
make -j$(nproc)
rm -rf data
echo 49 2 0 | bin/cs2cs "WGS84 + EGM96 height" EPSG:4979
echo 49 2 0 | bin/cs2cs "WGS84 + EGM96 height" EPSG:4979  | grep 44.643 >/dev/null || (echo "Expected 49dN 2dE 44.643 as a result" && /bin/false)
echo 0 0 0 | bin/cct +init=ITRF2000:ITRF96
echo 0 0 0 | bin/cct +init=ITRF2000:ITRF96 | grep 0.0067 >/dev/null || (echo "Expected 0.0067 0.0061 -0.0185 as a result" && /bin/false)

cd ..

ccache -s

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && tar czf "$WORK_DIR/ccache.tar.gz" .cache)
