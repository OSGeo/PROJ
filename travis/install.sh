#!/bin/bash

set -e

UNAME="$(uname)" || UNAME=""
if test "x${NPROC}" = "x"; then
    if test "${UNAME}" = "Linux" ; then
        NPROC=$(nproc);
    elif test "${UNAME}" = "Darwin" ; then
        NPROC=$(sysctl -n hw.ncpu);
    fi
    if test "x${NPROC}" = "x"; then
        NPROC=2;
    fi
fi
echo "NPROC=${NPROC}"
export MAKEFLAGS="-j ${NPROC}"

# Use ccache if it's available
if command -v ccache &> /dev/null
then
    USE_CCACHE=ON
    ccache -s
else
    USE_CCACHE=OFF
fi

if test "x${CMAKE_BUILD_TYPE}" = "x"; then
    CMAKE_BUILD_TYPE=Release
fi

# For some odd reason the tar xzvf $TAR_FILENAME doesn't work on Travis-CI ...
if test "$TRAVIS" = ""; then
    echo "Make dist tarball, and check consistency"
    mkdir build_dist
    cd build_dist
    cmake -D BUILD_TESTING=OFF ..
    make dist

    TAR_FILENAME=$(ls *.tar.gz)
    TAR_DIRECTORY=$(basename $TAR_FILENAME .tar.gz)
    mkdir ../build_from_dist
    cd ../build_from_dist
    tar xvzf ../build_dist/$TAR_FILENAME

    # continue build from dist tarball
    cd $TAR_DIRECTORY
fi

# There's a nasty #define CS in a Solaris system header. Avoid being caught about that again
CXXFLAGS="-DCS=do_not_use_CS_for_solaris_compat $CXXFLAGS"

echo "Build shared ${CMAKE_BUILD_TYPE} configuration from generated tarball"
mkdir shared_build
cd shared_build
cmake \
  -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
  -D USE_CCACHE=${USE_CCACHE} \
  -D BUILD_SHARED_LIBS=ON \
  -D CMAKE_INSTALL_PREFIX=/tmp/proj_shared_install_from_dist \
  ..
make

if [ "$(uname)" == "Linux" -a -f lib/libproj.so ]; then
if objdump -TC "lib/libproj.so" | grep "elf64-x86-64">/dev/null; then
    echo "Checking exported symbols..."
    cat $TRAVIS_BUILD_DIR/scripts/reference_exported_symbols.txt | sort > /tmp/reference_exported_symbols.txt
    $TRAVIS_BUILD_DIR/scripts/dump_exported_symbols.sh lib/libproj.so | sort > /tmp/got_symbols.txt
    diff -u /tmp/reference_exported_symbols.txt /tmp/got_symbols.txt || (echo "Difference(s) found in exported symbols. If intended, refresh scripts/reference_exported_symbols.txt with 'scripts/dump_exported_symbols.sh lib/libproj.so > scripts/reference_exported_symbols.txt'"; exit 1)
fi
fi

ctest
make install
# find /tmp/proj_shared_install_from_dist
$TRAVIS_BUILD_DIR/test/postinstall/test_cmake.sh /tmp/proj_shared_install_from_dist shared
$TRAVIS_BUILD_DIR/test/postinstall/test_autotools.sh /tmp/proj_shared_install_from_dist shared

echo "Build static ${CMAKE_BUILD_TYPE} configuration from generated tarball"
cd ..
mkdir static_build
cd static_build
cmake \
  -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
  -D USE_CCACHE=${USE_CCACHE} \
  -D BUILD_SHARED_LIBS=OFF \
  -D CMAKE_INSTALL_PREFIX=/tmp/proj_static_install_from_dist \
  ..
make

ctest
make install
# find /tmp/proj_static_install_from_dist
$TRAVIS_BUILD_DIR/test/postinstall/test_cmake.sh /tmp/proj_static_install_from_dist static
$TRAVIS_BUILD_DIR/test/postinstall/test_autotools.sh /tmp/proj_static_install_from_dist static

echo "Run PROJJSON tests only with shared configuration"

test_projjson(){
  printf "Testing PROJJSON output with $* ... "
  case "$1" in
    \+*)
      /tmp/proj_shared_install_from_dist/bin/projinfo "$@" -o PROJJSON -q > out.json ;;
    *)
      /tmp/proj_shared_install_from_dist/bin/projinfo $* -o PROJJSON -q > out.json ;;
  esac
  # cat out.json
  if [ $(jsonschema --version) = "3.2.0" ]; then
    # workaround for this version, which does not validate UTF-8
    tr -cd '\11\12\15\40-\176' < out.json > out.ascii
    mv out.ascii out.json
  fi
  jsonschema -i out.json /tmp/proj_shared_install_from_dist/share/proj/projjson.schema.json
  ret=$?
  if [ $ret != 0 ]; then
    return $ret
  else
    echo "Valid!"
  fi
  /tmp/proj_shared_install_from_dist/bin/projinfo @out.json -o PROJJSON -q > out2.json
  diff -u out.json out2.json
  ret=$?
  rm -f out.json out2.json
  return $ret
}

test_projjson EPSG:32631
test_projjson EPSG:4326+3855
test_projjson "+proj=longlat +ellps=GRS80 +nadgrids=@foo +type=crs"
test_projjson -s EPSG:3111 -t GDA2020

cd ..

echo "Check that we can retrieve the resource directory in a relative way after renaming the installation prefix"

mkdir /tmp/proj_shared_install_from_dist_renamed
mkdir /tmp/proj_static_install_from_dist_renamed
mv /tmp/proj_shared_install_from_dist /tmp/proj_shared_install_from_dist_renamed/subdir
mv /tmp/proj_static_install_from_dist /tmp/proj_static_install_from_dist_renamed/subdir
set +e
/tmp/proj_shared_install_from_dist_renamed/subdir/bin/projsync --source-id ? --dry-run --system-directory 2>/dev/null 1>shared.out
/tmp/proj_static_install_from_dist_renamed/subdir/bin/projsync --source-id ? --dry-run --system-directory 2>/dev/null 1>static.out
set -e
if [ "$TRAVIS_OS_NAME" == "osx" ]; then
    # on macOS /tmp is a symblink to /private/tmp only for the shared build
    INST=/private/tmp
else
    INST=/tmp
fi
cat shared.out
grep "Downloading from https://cdn.proj.org into $INST/proj_shared_install_from_dist_renamed/subdir/share/proj" shared.out
cat static.out
grep "Downloading from https://cdn.proj.org into /tmp/proj_static_install_from_dist_renamed/subdir/share/proj" static.out
rm shared.out static.out

sed -i'.bak' -e '1c\
prefix='"$INST"'/proj_shared_install_from_dist_renamed/subdir' $INST/proj_shared_install_from_dist_renamed/subdir/lib/pkgconfig/proj.pc
# cat $INST/proj_shared_install_from_dist_renamed/subdir/lib/pkgconfig/proj.pc

sed -i'.bak' -e '1c\
prefix=/tmp/proj_static_install_from_dist_renamed/subdir' /tmp/proj_static_install_from_dist_renamed/subdir/lib/pkgconfig/proj.pc
# cat /tmp/proj_static_install_from_dist_renamed/subdir/lib/pkgconfig/proj.pc

if [ $BUILD_NAME = "linux_gcc" ]; then
    $TRAVIS_BUILD_DIR/test/postinstall/test_autotools.sh /tmp/proj_shared_install_from_dist_renamed/subdir shared
    PROJ_LIB=/tmp/proj_static_install_from_dist_renamed/subdir/share/proj $TRAVIS_BUILD_DIR/test/postinstall/test_autotools.sh /tmp/proj_static_install_from_dist_renamed/subdir static
else
    echo "Skipping test_autotools.sh test for $BUILD_NAME"
fi

$TRAVIS_BUILD_DIR/test/postinstall/test_cmake.sh /tmp/proj_shared_install_from_dist_renamed/subdir shared
PROJ_LIB=/tmp/proj_static_install_from_dist_renamed/subdir/share/proj $TRAVIS_BUILD_DIR/test/postinstall/test_cmake.sh /tmp/proj_static_install_from_dist_renamed/subdir static

if [ "$BUILD_NAME" != "linux_gcc8" -a "$BUILD_NAME" != "linux_gcc_32bit" ]; then
    echo "Build PROJ as a subproject"
    mkdir proj_as_subproject
    cd proj_as_subproject
    mkdir external
    ln -s $PWD/.. external/PROJ

    echo '#include "proj.h"' > mytest.c
    echo 'int main() { proj_info(); return 0; }' >> mytest.c

    echo 'cmake_minimum_required(VERSION 3.9)' > CMakeLists.txt
    echo 'project(mytest)' >> CMakeLists.txt
    echo 'add_subdirectory(external/PROJ)' >> CMakeLists.txt
    echo 'add_executable(mytest mytest.c)' >> CMakeLists.txt
    echo 'target_include_directories(mytest PRIVATE $<TARGET_PROPERTY:PROJ::proj,INTERFACE_INCLUDE_DIRECTORIES>)' >> CMakeLists.txt
    echo 'target_link_libraries(mytest PRIVATE PROJ::proj)' >> CMakeLists.txt

    mkdir build_cmake
    cd build_cmake
    cmake -D USE_CCACHE=${USE_CCACHE} ..
    make

    # return to root
    cd ../..
    if test "$TRAVIS" = ""; then
        cd ../..
    fi

    echo "Build coverage as in-source build"
    # There's an issue with the clang on Travis + coverage + cpp code
    if [ "$BUILD_NAME" != "linux_clang" ]; then
        # build with grids and coverage
        if [ "$TRAVIS_OS_NAME" == "osx" ]; then
            cmake \
              -D CMAKE_BUILD_TYPE=Debug \
              -D USE_CCACHE=${USE_CCACHE} \
              -D CMAKE_C_FLAGS="--coverage" \
              -D CMAKE_CXX_FLAGS="--coverage" \
              . ;
        else
            LDFLAGS="$LDFLAGS -lgcov" cmake \
              -D CMAKE_BUILD_TYPE=Debug \
              -D USE_CCACHE=${USE_CCACHE} \
              -D CMAKE_C_FLAGS="$CFLAGS --coverage" \
              -D CMAKE_CXX_FLAGS="$CXXFLAGS --coverage" \
              . ;
        fi
    else
        cmake \
          -D CMAKE_BUILD_TYPE=${CMAKE_BUILD_TYPE} \
          -D USE_CCACHE=${USE_CCACHE} \
          . ;
    fi
    make
    ctest
fi

if [ "${USE_CCACHE}" = "ON" ]; then
    ccache -s
fi
