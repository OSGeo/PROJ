#!/bin/bash

# This script is meant to be run by
# https://github.com/google/oss-fuzz/blob/master/projects/proj.4/Dockerfile

set -e

if [ "$SRC" == "" ]; then
    echo "SRC env var not defined"
    exit 1
fi

if [ "$OUT" == "" ]; then
    echo "OUT env var not defined"
    exit 1
fi

if [ "$CXX" == "" ]; then
    echo "CXX env var not defined"
    exit 1
fi

if [ "$LIB_FUZZING_ENGINE" = "" ]; then
    export LIB_FUZZING_ENGINE=-lFuzzingEngine
fi

I386_PACKAGES="zlib1g-dev:i386 libssl-dev:i386 libsqlite3-dev:i386"
X64_PACKAGES="zlib1g-dev libssl-dev libsqlite3-dev"

if [ "$ARCHITECTURE" = "i386" ]; then
    apt-get install -y $I386_PACKAGES
else
    apt-get install -y $X64_PACKAGES
fi

# build libcurl.a (builing against Ubuntu libcurl.a doesn't work easily)
cd curl
autoreconf -i
./configure --disable-shared --with-openssl --prefix=$SRC/install
make clean -s
make -j$(nproc) -s
make install
cd ..

# build libtiff.a
cd libtiff
./autogen.sh
./configure --disable-shared --prefix=$SRC/install
make -j$(nproc)
make install
cd ..

mkdir build
cd build
cmake .. -DBUILD_SHARED_LIBS:BOOL=OFF \
        -DCURL_INCLUDE_DIR:PATH="$SRC/install/include" \
        -DCURL_LIBRARY_RELEASE:FILEPATH="$SRC/install/lib/libcurl.a" \
        -DTIFF_INCLUDE_DIR:PATH="$SRC/install/include" \
        -DTIFF_LIBRARY_RELEASE:FILEPATH="$SRC/install/lib/libtiff.a" \
        -DCMAKE_INSTALL_PREFIX=$SRC/install \
        -DBUILD_APPS:BOOL=OFF \
        -DBUILD_TESTING:BOOL=OFF
make clean -s
make -j$(nproc) -s
make install
cd ..

EXTRA_LIBS="-lpthread -Wl,-Bstatic -lsqlite3 -L$SRC/install/lib -ltiff -lcurl -lssl -lcrypto -lz -Wl,-Bdynamic"

build_fuzzer()
{
    fuzzerName=$1
    sourceFilename=$2
    shift
    shift
    echo "Building fuzzer $fuzzerName"
    $CXX $CXXFLAGS -std=c++11 -fvisibility=hidden -Isrc -Iinclude \
        $sourceFilename $* -o $OUT/$fuzzerName \
        $LIB_FUZZING_ENGINE "$SRC/install/lib/libproj.a" $EXTRA_LIBS
}

build_fuzzer proj_crs_to_crs_fuzzer test/fuzzers/proj_crs_to_crs_fuzzer.cpp

echo "[libfuzzer]" > $OUT/proj_crs_to_crs_fuzzer.options
echo "max_len = 10000" >> $OUT/proj_crs_to_crs_fuzzer.options

cp -r data/* $OUT
