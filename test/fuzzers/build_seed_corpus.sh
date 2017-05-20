#!/bin/bash

set -e

if [ "$OUT" == "" ]; then
    echo "OUT env var not defined"
    exit 1
fi

SRC_DIR=$(dirname $0)/../..

cp -r $SRC_DIR/nad/* $OUT
