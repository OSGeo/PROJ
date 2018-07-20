#!/bin/bash

set -e

# Download grid files to nad/
wget http://download.osgeo.org/proj/proj-datumgrid-1.7.zip
cd nad
unzip -o ../proj-datumgrid-1.7.zip
GRIDDIR=`pwd`
echo $GRIDDIR
cd ..

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
make install
PROJ_LIB=$GRIDDIR make check
find /tmp/proj_autoconf_install_from_dist_all
cd ..

# cmake build from generated tarball
mkdir build_cmake
cd build_cmake
cmake .. -DCMAKE_INSTALL_PREFIX=/tmp/proj_cmake_install
make -j3
make install
PROJ_LIB=$GRIDDIR ctest
find /tmp/proj_cmake_install
cd ..

# return to root
cd ../..

# cmake build with grids
mkdir build_cmake_nad
cd build_cmake_nad
cmake .. -DCMAKE_INSTALL_PREFIX=/tmp/proj_cmake_install_nad
make -j3
make install
find /tmp/proj_cmake_install_nad
cd ..
# autoconf build with grids
mkdir build_autoconf_nad
cd build_autoconf_nad
../configure --prefix=/tmp/proj_autoconf_install_nad
make -j3
make install
find /tmp/proj_autoconf_install_nad
PROJ_LIB=$GRIDDIR make check
cd src
make multistresstest
make test228
cd ..
PROJ_LIB=../nad src/multistresstest
cd ..
# autoconf build with grids and coverage
if [ $TRAVIS_OS_NAME == "osx" ]; then
    CFLAGS="--coverage" ./configure;
else
    CFLAGS="--coverage" LDFLAGS="-lgcov" ./configure;
fi
make -j3
PROJ_LIB=$GRIDDIR make check

# Rerun tests without grids not included in proj-datumgrid
rm -v ${GRIDDIR}/egm96_15.gtx
PROJ_LIB=$GRIDDIR make check

mv src/.libs/*.gc* src
