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

for i in "$TOPDIR"/include/proj/*.hh "$TOPDIR"/src/*.cpp "$TOPDIR"/test/unit/test*.cpp; do
    "$SCRIPT_DIR"/reformat.sh "$i";
done
