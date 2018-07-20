#!/bin/bash

set -e

export CCACHE_CPP2=yes

# prepare build files
./autogen.sh
# autoconf build
mkdir build_autoconf
cd build_autoconf
CC="ccache x86_64-w64-mingw32-gcc" CXX="ccache x86_64-w64-mingw32-g++" LD=x86_64-w64-mingw32-ld ../configure --host=x86_64-w64-mingw32 --prefix=/tmp/proj_autoconf_install
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
CC="ccache x86_64-w64-mingw32-gcc" CXX="ccache x86_64-w64-mingw32-g++" LD=x86_64-w64-mingw32-ld ../configure --host=x86_64-w64-mingw32 --prefix=/tmp/proj_autoconf_install_nad
make -j3
make install
find /tmp/proj_autoconf_install_nad
#make check
cd src
make multistresstest.exe
make test228.exe
cd ..
PROJ_LIB=../nad src/multistresstest.exe -num_iterations 100000
cd ..

