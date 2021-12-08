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

# prepare build files
./autogen.sh

# autoconf build
mkdir build_autoconf
cd build_autoconf
../configure
make dist-all >/dev/null
# Check consistency of generated tarball
TAR_FILENAME=`ls *.tar.gz`
TAR_DIRECTORY=`basename $TAR_FILENAME .tar.gz`
tar xvzf $TAR_FILENAME
cd ..

# compare with CMake's dist
mkdir build_cmake
cd build_cmake
cmake -DBUILD_TESTING=OFF ..
make dist
tar xzf $TAR_FILENAME
cd ..
diff -qr build_autoconf/$TAR_DIRECTORY build_cmake/$TAR_DIRECTORY || true

# continue build from autoconf
cd build_autoconf/$TAR_DIRECTORY

# There's a nasty #define CS in a Solaris system header. Avoid being caught about that again
CXXFLAGS="-DCS=do_not_use_CS_for_solaris_compat $CXXFLAGS"

# autoconf build from generated tarball
mkdir build_autoconf
cd build_autoconf
../configure --prefix=/tmp/proj_autoconf_install_from_dist_all

make >/dev/null

if [ "$(uname)" == "Linux" -a -f src/.libs/libproj.so ]; then
if objdump -TC "src/.libs/libproj.so" | grep "elf64-x86-64">/dev/null; then
    echo "Checking exported symbols..."
    cat $TRAVIS_BUILD_DIR/scripts/reference_exported_symbols.txt | sort > /tmp/reference_exported_symbols.txt
    $TRAVIS_BUILD_DIR/scripts/dump_exported_symbols.sh src/.libs/libproj.so | sort > /tmp/got_symbols.txt
    diff -u /tmp/reference_exported_symbols.txt /tmp/got_symbols.txt || (echo "Difference(s) found in exported symbols. If intended, refresh scripts/reference_exported_symbols.txt with 'scripts/dump_exported_symbols.sh src/.libs/libproj.so > scripts/reference_exported_symbols.txt'"; exit 1)
fi
fi

make check
make install
find /tmp/proj_autoconf_install_from_dist_all
if [ $BUILD_NAME = "linux_gcc" ]; then
    $TRAVIS_BUILD_DIR/test/postinstall/test_autotools.sh /tmp/proj_autoconf_install_from_dist_all shared
    $TRAVIS_BUILD_DIR/test/postinstall/test_autotools.sh /tmp/proj_autoconf_install_from_dist_all static
elif [ $BUILD_NAME = "osx" ]; then
    # skip static builds, as macOS shows: ld: unknown option: -Bstatic
    $TRAVIS_BUILD_DIR/test/postinstall/test_autotools.sh /tmp/proj_autoconf_install_from_dist_all shared
else
    echo "Skipping test_autotools.sh test for $BUILD_NAME"
fi

/tmp/proj_autoconf_install_from_dist_all/bin/projinfo EPSG:32631 -o PROJJSON -q > out.json
cat out.json
echo "Validating JSON"
jsonschema -i out.json /tmp/proj_autoconf_install_from_dist_all/share/proj/projjson.schema.json && echo "Valid !"

/tmp/proj_autoconf_install_from_dist_all/bin/projinfo EPSG:4326+3855 -o PROJJSON -q > out.json
cat out.json
echo "Validating JSON"
jsonschema -i out.json /tmp/proj_autoconf_install_from_dist_all/share/proj/projjson.schema.json && echo "Valid !"

/tmp/proj_autoconf_install_from_dist_all/bin/projinfo "+proj=longlat +ellps=GRS80 +nadgrids=@foo +type=crs" -o PROJJSON -q > out.json
cat out.json
echo "Validating JSON"
jsonschema -i out.json /tmp/proj_autoconf_install_from_dist_all/share/proj/projjson.schema.json && echo "Valid !"
/tmp/proj_autoconf_install_from_dist_all/bin/projinfo @out.json -o PROJJSON -q > out2.json
diff -u out.json out2.json

/tmp/proj_autoconf_install_from_dist_all/bin/projinfo -s EPSG:3111 -t GDA2020 -o PROJJSON -o PROJJSON -q > out.json
cat out.json
echo "Validating JSON"
jsonschema -i out.json /tmp/proj_autoconf_install_from_dist_all/share/proj/projjson.schema.json && echo "Valid !"
/tmp/proj_autoconf_install_from_dist_all/bin/projinfo @out.json -o PROJJSON -q > out2.json
diff -u out.json out2.json

# Test make clean target
make clean > /dev/null

cd ..

if [ $TRAVIS_OS_NAME != "osx" ]; then
    # Check that we can retrieve the resource directory in a relative way after renaming the installation prefix
    mkdir /tmp/proj_autoconf_install_from_dist_all_renamed
    mv /tmp/proj_autoconf_install_from_dist_all /tmp/proj_autoconf_install_from_dist_all_renamed/subdir
    LD_LIBRARY_PATH=/tmp/proj_autoconf_install_from_dist_all_renamed/subdir/lib /tmp/proj_autoconf_install_from_dist_all_renamed/subdir/bin/projsync --source-id ? --dry-run --system-directory || /bin/true
    LD_LIBRARY_PATH=/tmp/proj_autoconf_install_from_dist_all_renamed/subdir/lib /tmp/proj_autoconf_install_from_dist_all_renamed/subdir/bin/projsync --source-id ? --dry-run --system-directory 2>/dev/null | grep "Downloading from https://cdn.proj.org into /tmp/proj_autoconf_install_from_dist_all_renamed/subdir/share/proj"
    sed -i '1cprefix=/tmp/proj_autoconf_install_from_dist_all_renamed/subdir' /tmp/proj_autoconf_install_from_dist_all_renamed/subdir/lib/pkgconfig/proj.pc
    if [ $BUILD_NAME = "linux_gcc" ]; then
        $TRAVIS_BUILD_DIR/test/postinstall/test_autotools.sh /tmp/proj_autoconf_install_from_dist_all_renamed/subdir shared
        PROJ_LIB=/tmp/proj_autoconf_install_from_dist_all_renamed/subdir/share/proj $TRAVIS_BUILD_DIR/test/postinstall/test_autotools.sh /tmp/proj_autoconf_install_from_dist_all_renamed/subdir static
    else
        echo "Skipping test_autotools.sh test for $BUILD_NAME"
    fi
fi

if [ "$BUILD_NAME" != "linux_gcc8" -a "$BUILD_NAME" != "linux_gcc_32bit" ]; then

    cmake --version

    # Build PROJ as a subproject
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
    cmake .. -DCMAKE_BUILD_TYPE=Debug
    VERBOSE=1 make >/dev/null
    cd ../..

    # Use ccache if it's available
    if command -v ccache &> /dev/null
    then
        USE_CCACHE=ON
        ccache -s
    else
        USE_CCACHE=OFF
    fi

    # Regular build
    mkdir build_cmake
    cd build_cmake
    cmake .. -DCMAKE_INSTALL_PREFIX=/tmp/proj_cmake_install -DUSE_CCACHE=${USE_CCACHE}
    make >/dev/null
    if [ "${USE_CCACHE}" = "ON" ]; then
        ccache -s
    fi

    make install >/dev/null
    ctest
    find /tmp/proj_cmake_install
    if [ $BUILD_NAME = "linux_gcc" ] || [ $BUILD_NAME = "osx" ]; then
        $TRAVIS_BUILD_DIR/test/postinstall/test_cmake.sh /tmp/proj_cmake_install shared
        $TRAVIS_BUILD_DIR/test/postinstall/test_autotools.sh /tmp/proj_cmake_install shared
    else
        echo "Skipping test_autotools.sh test for $BUILD_NAME"
    fi
    cd ..

    if [ $TRAVIS_OS_NAME != "osx" ]; then
        # Check that we can retrieve the resource directory in a relative way after renaming the installation prefix
        mkdir /tmp/proj_cmake_install_renamed
        mv /tmp/proj_cmake_install /tmp/proj_cmake_install_renamed/subdir
        /tmp/proj_cmake_install_renamed/subdir/bin/projsync --source-id ? --dry-run --system-directory || /bin/true
        /tmp/proj_cmake_install_renamed/subdir/bin/projsync --source-id ? --dry-run --system-directory 2>/dev/null | grep "Downloading from https://cdn.proj.org into /tmp/proj_cmake_install_renamed/subdir/share/proj"
    fi

    # return to root
    cd ../..

    # There's an issue with the clang on Travis + coverage + cpp code
    if [ "$BUILD_NAME" != "linux_clang" ]; then
        # autoconf build with grids and coverage
        if [ "$TRAVIS_OS_NAME" == "osx" ]; then
            CFLAGS="--coverage" CXXFLAGS="--coverage" ./configure;
        else
            CFLAGS="$CFLAGS --coverage" CXXFLAGS="$CXXFLAGS --coverage" LDFLAGS="$LDFLAGS -lgcov" ./configure;
        fi
    else
        ./configure
    fi
    make >/dev/null
    make check
fi
