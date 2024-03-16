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

sudo apt-get install -y --no-install-recommends \
    binutils-mingw-w64-x86-64 \
    gcc-mingw-w64-x86-64 \
    g++-mingw-w64-x86-64 \
    g++-mingw-w64 \
    mingw-w64-tools \
    wine-stable \
    cmake make ccache \
    sqlite3 \
    curl ca-certificates

# Select posix/pthread for std::mutex
update-alternatives --set x86_64-w64-mingw32-gcc /usr/bin/x86_64-w64-mingw32-gcc-posix
update-alternatives --set x86_64-w64-mingw32-g++ /usr/bin/x86_64-w64-mingw32-g++-posix

ccache -M 500M
ccache -s

MINGW_ARCH=x86_64-w64-mingw32
MINGW_PREFIX=/usr/lib/gcc/$MINGW_ARCH/10-posix

export CC="$MINGW_ARCH-gcc"
export CXX="$MINGW_ARCH-g++"
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
# ln -s $MINGW_PREFIX/libgcc_s_sjlj-1.dll $WINE_SYSDIR
ln -s /usr/$MINGW_ARCH/lib/libwinpthread-1.dll $WINE_SYSDIR

# build zlib
wget https://github.com/madler/zlib/archive/v1.2.11.tar.gz
tar xzf v1.2.11.tar.gz
(cd zlib-1.2.11 && \
  sudo make install -f win32/Makefile.gcc SHARED_MODE=1 \
    PREFIX=${MINGW_ARCH}- \
    prefix=/usr/${MINGW_ARCH} \
    DESTDIR=/usr/${MINGW_ARCH}/ \
    BINARY_PATH=bin INCLUDE_PATH=include LIBRARY_PATH=lib)
ln -s /usr/$MINGW_ARCH/bin/zlib1.dll $WINE_SYSDIR

# build libtiff
wget http://download.osgeo.org/libtiff/tiff-4.1.0.tar.gz
tar xzf tiff-4.1.0.tar.gz
(cd tiff-4.1.0 && ./configure --host=$MINGW_ARCH --prefix=/usr/$MINGW_ARCH && make && sudo make install)
ln -s /usr/$MINGW_ARCH/bin/libtiff-5.dll $WINE_SYSDIR

# build sqlite3
wget https://sqlite.org/2020/sqlite-autoconf-3330000.tar.gz
tar xzf sqlite-autoconf-3330000.tar.gz
# Build with SQLITE_DQS=0 to ensure we properly use single quotes and double quotes (cf issue #2480)
(cd sqlite-autoconf-3330000 &&
CFLAGS="-DSQLITE_DQS=0" ./configure --host=$MINGW_ARCH --prefix=/usr/$MINGW_ARCH && make && sudo make install)
ln -s /usr/$MINGW_ARCH/bin/libsqlite3-0.dll $WINE_SYSDIR

# build proj
rm -rf build
mkdir build
cd build
cmake -G "Unix Makefiles" \
    -D CMAKE_SYSTEM_NAME=Windows \
    -D CMAKE_CROSSCOMPILING_EMULATOR=/usr/bin/wine64 \
    -D CMAKE_C_COMPILER=/usr/bin/${MINGW_ARCH}-gcc \
    -D CMAKE_CXX_COMPILER=/usr/bin/${MINGW_ARCH}-g++ \
    -D CMAKE_C_FLAGS="-Werror" \
    -D CMAKE_CXX_FLAGS="-Werror" \
    -D CMAKE_BUILD_TYPE=Release \
    -D CMAKE_FIND_ROOT_PATH=/usr/${MINGW_ARCH} \
    -D CMAKE_INSTALL_PREFIX=/tmp/proj_install \
    -D ENABLE_CURL=OFF \
    -D BUILD_PROJSYNC=OFF \
    -D USE_CCACHE=ON \
    -D PROJ_DB_CACHE_DIR=$HOME/.ccache \
    ..
make
# Run a subset of tests that should pass
ctest --output-on-failure -R "proj_test_cpp_api|geodesic-test|proj_errno_string_test|proj_angular_io_test|proj_context_test|pj_phi2_test|gie_self_tests|test_network|test_defmodel|test_tinshift|test_misc|test_fork"
# TODO: fix failing tests with .gie files; see #2168 and run
# PROJ_DATA=./data/for_tests wine64 ./bin/gie.exe ../test/gie/more_builtins.gie

make install

ccache -s

echo "Saving ccache..."
rm -f "$WORK_DIR/ccache.tar.gz"
(cd $HOME && test ! -e .ccache || tar czf "$WORK_DIR/ccache.tar.gz" .ccache)
