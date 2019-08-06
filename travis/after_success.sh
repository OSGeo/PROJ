#!/bin/bash

set -e

# coveralls falsely reports .c-files in the build directories as having 100% coverage so we exclude them
coveralls --extension .c --exclude build_autoconf --exclude build_cmake
echo "$TRAVIS_SECURE_ENV_VARS"
./travis/build_docs.sh
