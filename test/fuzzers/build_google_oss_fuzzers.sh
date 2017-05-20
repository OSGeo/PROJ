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
    $CXX $CXXFLAGS -std=c++11 -I$SRC_DIR/src \
        $sourceFilename $* -o $OUT/$fuzzerName \
        -lFuzzingEngine $SRC_DIR/src/.libs/libproj.a -lpthread $EXTRA_LIBS
}

build_fuzzer standard_fuzzer $(dirname $0)/standard_fuzzer.cpp
