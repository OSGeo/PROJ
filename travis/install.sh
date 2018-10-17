#!/bin/bash

set -e

# Download grid files
wget http://download.osgeo.org/proj/proj-datumgrid-1.7.zip

# prepare build files
./autogen.sh

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
../configure --prefix=/tmp/proj_autoconf_install_from_dist_all
make -j3
make check
make install
find /tmp/proj_autoconf_install_from_dist_all
cd ..

# cmake build from generated tarball
mkdir build_cmake
cd build_cmake
cmake .. -DCMAKE_INSTALL_PREFIX=/tmp/proj_cmake_install
make -j3
make install
# The cmake build is not able to generate the null file, so copy it at hand
cp /tmp/proj_autoconf_install_from_dist_all/share/proj/null /tmp/proj_cmake_install/share/proj
PROJ_LIB=/tmp/proj_cmake_install/share/proj ctest
find /tmp/proj_cmake_install
cd ..

# return to root
cd ../..

# Install grid files
(cd data && unzip -o ../proj-datumgrid-1.7.zip)

# autoconf build with grids
mkdir build_autoconf_grids
cd build_autoconf_grids
../configure --prefix=/tmp/proj_autoconf_install_grids
make -j3
make check
(cd src && make multistresstest && make test228)
PROJ_LIB=../data src/multistresstest
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
make -j3
make check

# Rerun tests without grids not included in proj-datumgrid
rm -v data/egm96_15.gtx
make check

if [ "$BUILD_NAME" != "linux_clang" ]; then
    mv src/.libs/*.gc* src
fi
