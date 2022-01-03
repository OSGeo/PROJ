#!/bin/sh -e

# Post-install tests with CMake
#
# First required argument is the installed prefix, which
# is used to set CMAKE_PREFIX_PATH
# Second argument is either shared (default) or static
cd $(dirname $0)
. ./common.sh
main_setup $1 $2

echo "Running post-install tests with CMake (${BUILD_MODE})"


cmake_make_ctest(){
  rm -rf build
  mkdir build
  cd build

  cmake -DCMAKE_PREFIX_PATH=${prefix} -DUSE_PROJ_NAME=$1 ..
  VERBOSE=1 make
  ctest --output-on-failure

  cd ..
  rm -rf build
}

echo "Testing C app"
cd c_app
cmake_make_ctest PROJ
cmake_make_ctest PROJ4
cd ..

echo "Testing C++ app"
cd cpp_app
cmake_make_ctest PROJ
cmake_make_ctest PROJ4
cd ..

echo "Finished running post-install tests CMake (${BUILD_MODE})"
