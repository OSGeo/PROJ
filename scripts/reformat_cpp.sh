#!/bin/sh
set -eu

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

TOPDIR="$SCRIPT_DIR/.."

for i in "$TOPDIR"/include/proj/*.hpp "$TOPDIR"/include/proj/internal/*.hpp "$TOPDIR"/src/*.cpp "$TOPDIR"/test/unit/test*.cpp; do
    if ! echo "$i" | grep -q "lru_cache.hpp"; then
        "$SCRIPT_DIR"/reformat.sh "$i";
    fi
done
