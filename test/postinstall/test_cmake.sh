#!/bin/sh

# Post-install tests with CMake
#
# First required argument is the installed prefix, which
# is used to set CMAKE_PREFIX_PATH

set -e

echo "Running post-install tests with CMake"

CMAKE_PREFIX_PATH=$1
if [ -z "$CMAKE_PREFIX_PATH" ]; then
    echo "First positional argument CMAKE_PREFIX_PATH required"
    exit 1
fi

echo "CMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH"

cd $(dirname $0)

cd testappprojinfo
rm -rf build

# Check CMake project name PROJ
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH -DUSE_PROJ_NAME=PROJ -DCMAKE_VERBOSE_MAKEFILE=ON ..
cmake --build .
ctest -VV .
cd ..
rm -rf build

# Check legacy CMake project name PROJ4
mkdir build
cd build
cmake -DCMAKE_PREFIX_PATH=$CMAKE_PREFIX_PATH -DUSE_PROJ_NAME=PROJ4 -DCMAKE_VERBOSE_MAKEFILE=ON ..
cmake --build .
ctest -VV .
cd ..
rm -rf build

cd ..
