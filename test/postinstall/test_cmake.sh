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

# CMake command options options differ since CMake 4.4
CMAKE_MAJOR_MINOR=$(cmake --version | grep -o '[[:digit:]]\+\.[[:digit:]]\+')
cmp_44=$(printf "4.4\n${CMAKE_MAJOR_MINOR}\n")
sorted_44=$(echo "$cmp_44" | sort -V)
if [ "$cmp_44" = "$sorted_44" ]; then  # CMake 4.4 or later
    CMAKE_OPTIONS="-Werror=author -Wno-error=deprecated --log-level=VERBOSE"
else  # Before CMake 4.4 - no way to supress "deprecated", so just warn
    CMAKE_OPTIONS="-Wdev --log-level=VERBOSE"
fi


echo "Running post-install tests with CMake (${BUILD_MODE}, ${TESTED_CONFIGS})"


cmake_make_ctest(){
  rm -rf build

  cmake ${CMAKE_OPTIONS} \
    -D CMAKE_PREFIX_PATH=${prefix} \
    -D CMAKE_COMPILE_WARNING_AS_ERROR=ON \
    -D USE_PROJ_NAME=$1 \
    -S . -B build

  cmake --build build --verbose

  (cd build && ctest --output-on-failure)

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
