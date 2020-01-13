#!/bin/bash

set -e

unset CC
unset CXX

export CCACHE_CPP2=yes
export PROJ_DB_CACHE_DIR="$HOME/.ccache"

MINGW_ARCH=x86_64-w64-mingw32
MINGW_PREFIX=/usr/lib/gcc/$MINGW_ARCH/5.3-posix

# prepare wine environment
WINE_SYSDIR=$HOME/.wine/drive_c/windows
wine64 cmd /c dir
ln -s $MINGW_PREFIX/libstdc++-6.dll $WINE_SYSDIR
ln -s $MINGW_PREFIX/libgcc_s_seh-1.dll $WINE_SYSDIR
ln -s $MINGW_PREFIX/libgcc_s_sjlj-1.dll $WINE_SYSDIR
ln -s /usr/$MINGW_ARCH/lib/libwinpthread-1.dll $WINE_SYSDIR

# build zlib
wget https://github.com/madler/zlib/archive/v1.2.11.tar.gz
tar xzf v1.2.11.tar.gz
(cd zlib-1.2.11 && sudo make install -fwin32/Makefile.gcc SHARED_MODE=1 PREFIX=x86_64-w64-mingw32- DESTDIR=/usr/$MINGW_ARCH/)
sudo mkdir -p  /usr/$MINGW_ARCH/include
sudo mkdir -p  /usr/$MINGW_ARCH/lib
sudo cp /usr/$MINGW_ARCH/*.h  /usr/$MINGW_ARCH/include
sudo cp /usr/$MINGW_ARCH/libz.* /usr/$MINGW_ARCH/lib
ln -s /usr/$MINGW_ARCH/zlib1.dll $WINE_SYSDIR

# build libtiff
wget https://download.osgeo.org/libtiff/tiff-4.1.0.tar.gz
tar xzf tiff-4.1.0.tar.gz
(cd tiff-4.1.0 && ./configure --host=$MINGW_ARCH --prefix=/usr/$MINGW_ARCH && make -j2 && sudo make install)
ln -s /usr/$MINGW_ARCH/bin/libtiff-5.dll $WINE_SYSDIR

# build sqlite3
wget https://sqlite.org/2018/sqlite-autoconf-3250100.tar.gz
tar xzf sqlite-autoconf-3250100.tar.gz
(cd sqlite-autoconf-3250100 && CC="ccache $MINGW_ARCH-gcc" CXX="ccache $MINGW_ARCH-g++" LD=$MINGW_ARCH-ld
./configure --host=$MINGW_ARCH --prefix=/usr/$MINGW_ARCH && make -j2 && sudo make install)
ln -s /usr/$MINGW_ARCH/bin/libsqlite3-0.dll $WINE_SYSDIR

# prepare build files
./autogen.sh
# autoconf build
mkdir build_autoconf
cd build_autoconf
CC="ccache $MINGW_ARCH-gcc" CXX="ccache $MINGW_ARCH-g++" LD=$MINGW_ARCH-ld ../configure --host=$MINGW_ARCH --prefix=/tmp/proj_autoconf_install --without-curl
make -j2
make install
make dist-all
find /tmp/proj_autoconf_install
(cd test; make -j2)
cp -r ../data/tests /tmp/proj_autoconf_install/share/proj
test/unit/test_cpp_api.exe
cd ..
# Now with grids
wget https://download.osgeo.org/proj/proj-datumgrid-1.5.zip
cd data
unzip -o ../proj-datumgrid-1.5.zip
cd ..
# autoconf build with grids
mkdir build_autoconf_grids
cd build_autoconf_grids
CC="ccache $MINGW_ARCH-gcc" CXX="ccache $MINGW_ARCH-g++" LD=$MINGW_ARCH-ld ../configure --host=$MINGW_ARCH --prefix=/tmp/proj_autoconf_install_nad --without-curl
make -j2
make install
find /tmp/proj_autoconf_install_nad
#make check
cd src
make multistresstest.exe
make test228.exe
cd ..
PROJ_LIB=../data src/multistresstest.exe -num_iterations 100000
cd ..

