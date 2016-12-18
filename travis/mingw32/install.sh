#!/bin/bash

set -e

export CCACHE_CPP2=yes

# prepare build files
./autogen.sh
# autoconf build
mkdir build_autoconf
cd build_autoconf
CC="ccache i586-mingw32msvc-gcc" LD=i586-mingw32msvc-ld ../configure --host=i586-mingw32msvc --prefix=/tmp/proj_autoconf_install
make -j3
make install
make dist-all
find /tmp/proj_autoconf_install
#make check
cd ..
# Now with grids
wget http://download.osgeo.org/proj/proj-datumgrid-1.5.zip
cd nad
unzip -o ../proj-datumgrid-1.5.zip
cd ..
# autoconf build with grids
mkdir build_autoconf_nad
cd build_autoconf_nad
CC="ccache i586-mingw32msvc-gcc" LD=i586-mingw32msvc-ld ../configure --host=i586-mingw32msvc --prefix=/tmp/proj_autoconf_install_nad
make -j3
make install
find /tmp/proj_autoconf_install_nad
#make check
cd src
make multistresstest.exe
make test228.exe
cd ..
PROJ_LIB=../nad src/multistresstest.exe
cd ..

