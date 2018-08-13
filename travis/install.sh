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
make install
# We have a small issue with out-of-tree builds where the null file is generated in the build directory, but other non-generated stuff is in $(top_srcdir)/nad
# Workaround this by using the install directory...
PROJ_LIB=/tmp/proj_autoconf_install_from_dist_all/share/proj make check
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
cd nad
unzip -o ../proj-datumgrid-1.7.zip
GRIDDIR=`pwd`
echo $GRIDDIR
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
