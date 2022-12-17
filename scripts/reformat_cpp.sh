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

if ! (clang-format --version 2>/dev/null | grep 10 >/dev/null); then
    echo "clang-format 10 not available. Running it from a Docker image";
    docker build -t proj_clang_format -f "$TOPDIR"/scripts/proj_clang_format/Dockerfile "$TOPDIR"/scripts/proj_clang_format
    UID=$(id -u "${USER}")
    GID=$(id -g "${USER}")
    exec docker run --rm -u "$UID:$GID" -v "$TOPDIR":"$TOPDIR" proj_clang_format "$TOPDIR"/scripts/reformat_cpp.sh
fi

for i in "$TOPDIR"/include/proj/*.hpp "$TOPDIR"/include/proj/internal/*.hpp \
         "$TOPDIR"/src/iso19111/*.cpp \
         "$TOPDIR"/src/iso19111/operation/*.cpp \
         "$TOPDIR"/src/iso19111/operation/*.hpp \
         "$TOPDIR"/test/unit/*.cpp \
         "$TOPDIR"/src/apps/projinfo.cpp "$TOPDIR"/src/apps/projsync.cpp \
         "$TOPDIR"/src/tracing.cpp "$TOPDIR"/src/grids.hpp "$TOPDIR"/src/grids.cpp \
         "$TOPDIR"/src/filemanager.hpp "$TOPDIR"/src/filemanager.cpp \
         "$TOPDIR"/src/networkfilemanager.cpp \
         "$TOPDIR"/src/sqlite3_utils.hpp "$TOPDIR"/src/sqlite3_utils.cpp \
         "$TOPDIR"/src/generic_inverse.cpp \
         "$TOPDIR"/src/transformations/defmodel.hpp \
         "$TOPDIR"/src/transformations/defmodel_exceptions.hpp \
         "$TOPDIR"/src/transformations/defmodel_impl.hpp \
         "$TOPDIR"/src/transformations/defmodel.cpp \
         "$TOPDIR"/src/transformations/tinshift.hpp \
         "$TOPDIR"/src/transformations/tinshift_exceptions.hpp \
         "$TOPDIR"/src/transformations/tinshift_impl.hpp \
         "$TOPDIR"/src/transformations/tinshift.cpp \
         "$TOPDIR"/src/transformations/gridshift.cpp \
         "$TOPDIR"/src/quadtree.hpp \
         ; do
    if ! echo "$i" | grep -q "lru_cache.hpp"; then
        "$SCRIPT_DIR"/reformat.sh "$i";
    fi
done
