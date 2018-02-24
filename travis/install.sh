#!/bin/bash

set -e

# Download grid files to nad/
wget http://download.osgeo.org/proj/proj-datumgrid-1.6.zip
cd nad
unzip -o ../proj-datumgrid-1.6.zip
wget http://download.osgeo.org/proj/vdatum/egm96_15/egm96_15.gtx
wget https://raw.githubusercontent.com/OSGeo/proj-datumgrid/master/BETA2007.gsb
GRIDDIR=`pwd`
echo $GRIDDIR
cd ..

# prepare build files
./autogen.sh
# cmake build
mkdir build_cmake
cd build_cmake
cmake .. -DCMAKE_INSTALL_PREFIX=/tmp/proj_cmake_install
make -j3
make install
find /tmp/proj_cmake_install
cd ..
# autoconf build
mkdir build_autoconf
cd build_autoconf
../configure --prefix=/tmp/proj_autoconf_install
make -j3
make install
make dist-all
find /tmp/proj_autoconf_install
PROJ_LIB=$GRIDDIR make check
# Check consistency of generated tarball
TAR_FILENAME=`ls *.tar.gz`
TAR_DIRECTORY=`basename $TAR_FILENAME .tar.gz`
tar xvzf $TAR_FILENAME
cd $TAR_DIRECTORY
./configure --prefix=/tmp/proj_autoconf_install_from_dist_all
make -j3
make install
PROJ_LIB=$GRIDDIR make check
CURRENT_PWD=`pwd`
cd /tmp/proj_autoconf_install
find | sort > /tmp/list_proj_autoconf_install.txt
cd /tmp/proj_autoconf_install_from_dist_all
find | sort > /tmp/list_proj_autoconf_install_from_dist_all.txt
cd $CURRENT_PWD
# The list of file is not identical. See http://lists.maptools.org/pipermail/proj/2015-September/007231.html
#diff -u /tmp/list_proj_autoconf_install.txt /tmp/list_proj_autoconf_install_from_dist_all.txt
cd ..
#
cd ..
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
