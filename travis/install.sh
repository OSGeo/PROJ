#!/bin/bash

set -e

UNAME="$(uname)" || UNAME=""
if test "${UNAME}" = "Linux" ; then
    NPROC=$(nproc);
elif test "${UNAME}" = "Darwin" ; then
    NPROC=$(sysctl -n hw.ncpu);
fi
if test "x${NPROC}" = "x"; then
    NPROC=2;
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
cd $TAR_DIRECTORY

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
if [ $BUILD_NAME = "linux_gcc" ] || [ $BUILD_NAME = "osx" ]; then
    $TRAVIS_BUILD_DIR/test/postinstall/test_pkg-config.sh /tmp/proj_autoconf_install_from_dist_all
else
    echo "Skipping test_pkg-config.sh test for $BUILD_NAME"
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
        $TRAVIS_BUILD_DIR/test/postinstall/test_pkg-config.sh /tmp/proj_autoconf_install_from_dist_all_renamed/subdir
    else
        echo "Skipping test_pkg-config.sh test for $BUILD_NAME"
    fi
fi

if [ "$BUILD_NAME" != "linux_gcc8" -a "$BUILD_NAME" != "linux_gcc_32bit" ]; then

    # cmake build from generated tarball
    mkdir build_cmake
    cd build_cmake
    cmake --version
    cmake .. -DCMAKE_INSTALL_PREFIX=/tmp/proj_cmake_install
    VERBOSE=1 make >/dev/null
    make install >/dev/null
    ctest
    find /tmp/proj_cmake_install
    if [ $BUILD_NAME = "linux_gcc" ] || [ $BUILD_NAME = "osx" ]; then
        $TRAVIS_BUILD_DIR/test/postinstall/test_cmake.sh /tmp/proj_cmake_install
    else
        echo "Skipping test_cmake.sh test for $BUILD_NAME"
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
