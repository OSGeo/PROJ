#!/bin/bash

set -e

dnf install -y cmake clang ccache ninja-build sqlite-devel libtiff-devel libcurl-devel diffutils wget gtest-devel python3-pyyaml
# For this build, install nlohmann-json so that the "use external"
# branch is tested; the other builds use the vendored copy.
dnf install -y nlohmann-json-devel

cd "$WORK_DIR"

if test -f "$WORK_DIR/ccache.tar.gz"; then
    echo "Restoring ccache..."
    (cd $HOME && tar xzf "$WORK_DIR/ccache.tar.gz")
    # temporary fix to move previous .cache to .ccache
    if [ -d "$HOME/.cache" ] && [ ! -d "$HOME/.ccache" ]; then
        mv "$HOME/.cache" "$HOME/.ccache"
    fi
else
    mkdir -p $HOME/.ccache
fi

export CCACHE_CPP2=yes

ccache -M 500M
ccache -s

# cmake-diagnostics added with CMake 4.4
# https://cmake.org/cmake/help/v4.4/manual/cmake-diagnostics.7.html
CMAKE_MAJOR_MINOR=$(cmake --version | grep -o '[[:digit:]]\+\.[[:digit:]]\+')
cmp_44=$(printf "4.4\n${CMAKE_MAJOR_MINOR}\n")
sorted_44=$(echo "$cmp_44" | sort -V)
if [ "$cmp_44" = "$sorted_44" ]; then  # CMake 4.4 or later
    cmake_diagnostics=author
else  # Before CMake 4.4
    cmake_diagnostics=dev
fi
CMAKE_OPTIONS="-Werror=${cmake_diagnostics} --log-level=VERBOSE"

export CMAKE_BUILD_PARALLEL_LEVEL=$(nproc)
export CTEST_PARALLEL_LEVEL=$(nproc)

echo "Build with -DEMBED_RESOURCE_FILES=ON"
CC=clang CXX=clang++ cmake ${CMAKE_OPTIONS} \
    -D EMBED_RESOURCE_FILES=ON \
    -D USE_CCACHE=ON \
    -D PROJ_DB_CACHE_DIR=$HOME/.ccache \
    -S . -B build

cmake --build build

(cd build && ctest --output-on-failure)
rm -rf build

# Try EMBED_RESOURCE_DIRECTORY option
wget -P grids https://raw.githubusercontent.com/OSGeo/PROJ-data/refs/heads/master/us_nga/us_nga_egm96_15.tif
wget -P grids https://cdn.proj.org/fi_nls_ykj_etrs35fin.gpkg

echo "Build with -DEMBED_RESOURCE_FILES=ON -DEMBED_RESOURCE_DIRECTORY=$PWD/grids"
CC=clang CXX=clang++ cmake ${CMAKE_OPTIONS} \
    -D EMBED_RESOURCE_FILES=ON \
    -D EMBED_RESOURCE_DIRECTORY=$PWD/grids \
    -D BUILD_TESTING=OFF \
    -D USE_CCACHE=ON \
    -D PROJ_DB_CACHE_DIR=$HOME/.ccache \
    -S . -B build

cmake --build build

cd build
rm -rf data
echo 49 2 0 | bin/cs2cs "WGS84 + EGM96 height" EPSG:4979
echo 49 2 0 | bin/cs2cs "WGS84 + EGM96 height" EPSG:4979  | grep 44.643 >/dev/null || (echo "Expected 49dN 2dE 44.643 as a result" && /bin/false)
echo 0 0 0 | bin/cct +init=ITRF2000:ITRF96
echo 0 0 0 | bin/cct +init=ITRF2000:ITRF96 | grep 0.0067 >/dev/null || (echo "Expected 0.0067 0.0061 -0.0185 as a result" && /bin/false)
echo 3432087 6995748 0 | bin/cct +proj=tinshift +file=fi_nls_ykj_etrs35fin.gpkg
echo 3432087 6995748 0 | bin/cct +proj=tinshift +file=fi_nls_ykj_etrs35fin.gpkg | grep 431943.0905 >/dev/null || (echo "Expected 431943.0905   6992816.7826 0 as a result" && /bin/false)
cd ..

echo "Build with -DEMBED_RESOURCE_FILES=ON -DEMBED_RESOURCE_DIRECTORY=$PWD/grids -DUSE_ONLY_EMBEDDED_RESOURCE_FILES=ON"
CC=clang CXX=clang++ cmake ${CMAKE_OPTIONS} \
    -D EMBED_RESOURCE_FILES=ON \
    -D EMBED_RESOURCE_DIRECTORY=$PWD/grids \
    -D USE_ONLY_EMBEDDED_RESOURCE_FILES=ON \
    -D USE_CCACHE=ON \
    -D PROJ_DB_CACHE_DIR=$HOME/.ccache \
    -S . -B build

cmake --build build

cd build
rm -rf data
echo 49 2 0 | bin/cs2cs "WGS84 + EGM96 height" EPSG:4979
echo 49 2 0 | bin/cs2cs "WGS84 + EGM96 height" EPSG:4979  | grep 44.643 >/dev/null || (echo "Expected 49dN 2dE 44.643 as a result" && /bin/false)
echo 0 0 0 | bin/cct +init=ITRF2000:ITRF96
echo 0 0 0 | bin/cct +init=ITRF2000:ITRF96 | grep 0.0067 >/dev/null || (echo "Expected 0.0067 0.0061 -0.0185 as a result" && /bin/false)
cd ..

ccache -s

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && tar czf "$WORK_DIR/ccache.tar.gz" .ccache)
