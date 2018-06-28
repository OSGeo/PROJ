#!/bin/bash

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

pushd "${TOPDIR}" > /dev/null || exit

# Check that doxygen runs warning free
doxygen > /tmp/docs_log.txt 2>&1
if grep -i warning /tmp/docs_log.txt; then
    echo "Doxygen warnings found" && cat /tmp/docs_log.txt && /bin/false;
else
    echo "No Doxygen warnings found";
fi

popd > /dev/null || exit

