#!/bin/bash

set -e

if [ "$BUILD_NAME" != "linux_clang" ]; then
    # coveralls falsely reports .c-files in the build directories as having 100% coverage so we exclude them
    coveralls --build-root src --extension .c --extension .cpp \
        --exclude build_autoconf --exclude build_cmake --exclude test --exclude examples \
        --exclude include/proj/internal/nlohmann
fi

echo "$TRAVIS_SECURE_ENV_VARS"
./travis/build_docs.sh
