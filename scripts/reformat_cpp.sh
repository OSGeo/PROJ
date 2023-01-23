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

TOPDIR="$(readlink -f $SCRIPT_DIR/..)"

if ! (clang-format --version 2>/dev/null | grep 10 >/dev/null); then
    echo "clang-format 10 not available. Running it from a Docker image";
    docker build -t proj_clang_format -f "$TOPDIR"/scripts/proj_clang_format/Dockerfile "$TOPDIR"/scripts/proj_clang_format
    UID=$(id -u "${USER}")
    GID=$(id -g "${USER}")
    exec docker run --rm -u "$UID:$GID" -v "$TOPDIR":"$TOPDIR" proj_clang_format "$TOPDIR"/scripts/reformat_cpp.sh
fi

for i in `find "$TOPDIR" -name "*.h" -o -name "*.hpp" -o -name "*.cpp" -o -name "*.c"`; do
    if echo "$i" | grep -q "lru_cache.hpp"; then
        continue
    fi
    if echo "$i" | grep -q "json.hpp"; then
        continue
    fi
    if echo "$i" | grep -q "generated"; then
        continue
    fi
    if echo "$i" | grep -q "geodesic"; then
        continue
    fi
    if echo "$i" | grep -q "geodtest"; then
        continue
    fi
    if echo "$i" | grep -q "geodsigntest"; then
        continue
    fi
    if echo "$i" | grep -q "nn.hpp"; then
        continue
    fi
    if echo "$i" | grep -q "googletest-src"; then
        continue
    fi
    if echo "$i" | grep -q "/build"; then
        continue
    fi
    if echo "$i" | grep -q "fix_typos"; then
        continue
    fi
    echo "reformat.sh $i"
    "$SCRIPT_DIR"/reformat.sh "$i"
done
