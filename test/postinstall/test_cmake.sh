#!/bin/sh -e

# Post-install tests with CMake
#
# First required argument is the installed prefix, which
# is used to set CMAKE_PREFIX_PATH
# Second argument is either shared (default) or static
# Third argument is either BOTH_CONFIG (default), to test both PROJ and PROJ4
# CMake configurations, or PROJ_CONFIG to only test PROJ.
cd $(dirname $0)
. ./common.sh
main_setup $1 $2

case $3 in
"" | BOTH_CONFIG) export TESTED_CONFIGS=BOTH_CONFIG ;;
     PROJ_CONFIG) export TESTED_CONFIGS=PROJ_CONFIG ;;
*)
  echo "Third argument must be either BOTH_CONFIG (default) or PROJ_CONFIG"
  exit 1 ;;
esac

echo "Running post-install tests with CMake (${BUILD_MODE}, ${TESTED_CONFIGS})"


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
if test "${TESTED_CONFIGS}" = "BOTH_CONFIG"; then
    cmake_make_ctest PROJ4
fi
cd ..

echo "Testing C++ app"
cd cpp_app
cmake_make_ctest PROJ
if test "${TESTED_CONFIGS}" = "BOTH_CONFIG"; then
    cmake_make_ctest PROJ4
fi
cd ..

echo "Finished running post-install tests CMake (${BUILD_MODE}, ${TESTED_CONFIGS})"
