#!/bin/bash

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

SRC_DIR=$(dirname $0)/../..

build_fuzzer()
{
    fuzzerName=$1
    sourceFilename=$2
    shift
    shift
    echo "Building fuzzer $fuzzerName"
    $CXX $CXXFLAGS -std=c++11 -fvisibility=hidden -I$SRC_DIR/src -I$SRC_DIR/include \
        $sourceFilename $* -o $OUT/$fuzzerName \
        -lFuzzingEngine $SRC_DIR/src/.libs/libproj.a -lpthread /usr/lib/x86_64-linux-gnu/libsqlite3.a $EXTRA_LIBS
}

build_fuzzer standard_fuzzer $(dirname $0)/standard_fuzzer.cpp
build_fuzzer proj_crs_to_crs_fuzzer $(dirname $0)/proj_crs_to_crs_fuzzer.cpp

echo "[libfuzzer]" > $OUT/standard_fuzzer.options
echo "max_len = 10000" >> $OUT/standard_fuzzer.options

echo "[libfuzzer]" > $OUT/proj_crs_to_crs_fuzzer.options
echo "max_len = 10000" >> $OUT/proj_crs_to_crs_fuzzer.options
