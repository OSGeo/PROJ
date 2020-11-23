#!/bin/sh

set -e

SCRIPT_DIR=$(dirname "$0")
case $SCRIPT_DIR in
    "/"*)
        ;;
    ".")
        SCRIPT_DIR=$(pwd)
        ;;
    *)
        SCRIPT_DIR=$(pwd)/$(dirname "$0")
        ;;
esac

apt-get update -y
DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends \
    sudo wget tzdata

USER=root
export USER

cd "$WORK_DIR"

if test -f "$WORK_DIR/ccache.tar.gz"; then
    echo "Restoring ccache..."
    (cd $HOME && tar xzf "$WORK_DIR/ccache.tar.gz")
fi

export PROJ_DB_CACHE_DIR="$HOME/.ccache"

sudo apt-get install -y --no-install-recommends \
    ccache \
    binutils-mingw-w64-x86-64 \
    gcc-mingw-w64-x86-64 \
    g++-mingw-w64-x86-64 \
    g++-mingw-w64 \
    mingw-w64-tools \
    wine-stable \
    make autoconf automake libtool zip \
    sqlite3 \
    curl ca-certificates

# Select posix/pthread for std::mutex
update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix
update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix

ccache -M 500M
ccache -s

MINGW_ARCH=x86_64-w64-mingw32
MINGW_PREFIX=/usr/lib/gcc/$MINGW_ARCH/7.3-posix

export CCACHE_CPP2=yes
export CC="ccache $MINGW_ARCH-gcc"
export CXX="ccache $MINGW_ARCH-g++"
export LD=$MINGW_ARCH-ld

UNAME="$(uname)" || UNAME=""
NPROC=$(nproc);
echo "NPROC=${NPROC}"
export MAKEFLAGS="-j ${NPROC}"

# prepare wine environment
export WINE_PREFIX="$HOME/.wine"
WINE_SYSDIR=$WINE_PREFIX/drive_c/windows
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
(cd tiff-4.1.0 && ./configure --host=$MINGW_ARCH --prefix=/usr/$MINGW_ARCH && make && sudo make install)
ln -s /usr/$MINGW_ARCH/bin/libtiff-5.dll $WINE_SYSDIR

# build sqlite3
wget https://sqlite.org/2018/sqlite-autoconf-3250100.tar.gz
tar xzf sqlite-autoconf-3250100.tar.gz
(cd sqlite-autoconf-3250100 && 
./configure --host=$MINGW_ARCH --prefix=/usr/$MINGW_ARCH && make && sudo make install)
ln -s /usr/$MINGW_ARCH/bin/libsqlite3-0.dll $WINE_SYSDIR

# prepare build files
./autogen.sh
# autoconf build
mkdir build_autoconf
cd build_autoconf
CFLAGS="-Werror" CXXFLAGS="-Werror" ../configure --host=$MINGW_ARCH --prefix=/tmp/proj_autoconf_install --without-curl
make
make install
make dist-all
find /tmp/proj_autoconf_install
(cd test; make)
cp -r ../data/tests /tmp/proj_autoconf_install/share/proj
cp ../data/tests/egm96_15_downsampled.gtx /tmp/proj_autoconf_install/share/proj/egm96_15.gtx
cp ../data/tests/ntv2_0_downsampled.gsb /tmp/proj_autoconf_install/share/proj/ntv2_0.gsb
wine64 test/unit/test_cpp_api.exe

ccache -s

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && tar czf "$WORK_DIR/ccache.tar.gz" .ccache)
