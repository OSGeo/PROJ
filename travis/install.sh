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

# Download grid files
wget http://download.osgeo.org/proj/proj-datumgrid-1.8.zip

# prepare build files
./autogen.sh
TOP_DIR=$PWD

# autoconf build
mkdir build_autoconf
cd build_autoconf
../configure
make dist-all
# Check consistency of generated tarball
TAR_FILENAME=`ls *.tar.gz`
TAR_DIRECTORY=`basename $TAR_FILENAME .tar.gz`
tar xvzf $TAR_FILENAME
cd $TAR_DIRECTORY

# autoconf build from generated tarball
mkdir build_autoconf
cd build_autoconf
if [ -f /usr/lib/jvm/java-7-openjdk-amd64/include/jni.h ]; then
    CXXFLAGS="-I/usr/lib/jvm/java-7-openjdk-amd64/include -I/usr/lib/jvm/java-7-openjdk-amd64/include/linux $CXXFLAGS" ../configure --prefix=/tmp/proj_autoconf_install_from_dist_all --with-jni
else
    ../configure --prefix=/tmp/proj_autoconf_install_from_dist_all
fi

make -j${NPROC}

if [ "$(uname)" == "Linux" -a -f src/.libs/libproj.so ]; then
    echo "Checking exported symbols..."
    ${TOP_DIR}/scripts/dump_exported_symbols.sh src/.libs/libproj.so > /tmp/got_symbols.txt
    diff -u ${TOP_DIR}/scripts/reference_exported_symbols.txt /tmp/got_symbols.txt || (echo "Difference(s) found in exported symbols. If intended, refresh scripts/reference_exported_symbols.txt with 'scripts/dump_exported_symbols.sh src/.libs/libproj.so > scripts/reference_exported_symbols.txt'"; exit 1)
fi

make check
make install
find /tmp/proj_autoconf_install_from_dist_all
cd ..

# cmake build from generated tarball
mkdir build_cmake
cd build_cmake
cmake .. -DCMAKE_INSTALL_PREFIX=/tmp/proj_cmake_install
VERBOSE=1 make -j${NPROC}
make install
# The cmake build is not able to generate the null file, so copy it at hand
cp /tmp/proj_autoconf_install_from_dist_all/share/proj/null /tmp/proj_cmake_install/share/proj
PROJ_LIB=/tmp/proj_cmake_install/share/proj ctest
find /tmp/proj_cmake_install
cd ..

# return to root
cd ../..

# Install grid files
(cd data && unzip -o ../proj-datumgrid-1.8.zip)

# autoconf build with grids
mkdir build_autoconf_grids
cd build_autoconf_grids
../configure --prefix=/tmp/proj_autoconf_install_grids
make -j${NPROC}
make check
(cd src && make multistresstest && make test228)
src/multistresstest
make install
find /tmp/proj_autoconf_install_grids
cd ..

# There's an issue with the clang on Travis + coverage + cpp code
if [ "$BUILD_NAME" != "linux_clang" ]; then
    # autoconf build with grids and coverage
    if [ $TRAVIS_OS_NAME == "osx" ]; then
        CFLAGS="--coverage" CXXFLAGS="--coverage" ./configure;
    else
        CFLAGS="$CFLAGS --coverage" CXXFLAGS="$CXXCFLAGS --coverage" LDFLAGS="$LDFLAGS -lgcov" ./configure;
    fi
else
    ./configure
fi
make -j${NPROC}
make check

# Rerun tests without grids not included in proj-datumgrid
rm -v data/egm96_15.gtx
make check

if [ "$BUILD_NAME" != "linux_clang" ]; then
    mv src/.libs/*.gc* src
    mv src/conversions/.libs/*.gc* src/conversions
    mv src/iso19111/.libs/*.gc* src/iso19111
    mv src/projections/.libs/*.gc* src/projections
    mv src/transformations/.libs/*.gc* src/transformations
fi
