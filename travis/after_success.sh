#!/bin/bash

set -e

# coveralls falsely reports .c-files in the build directories as having 100% coverage so we exclude them
coveralls --extension .c --exclude build_autoconf --exclude build_cmake
echo "$TRAVIS_SECURE_ENV_VARS"
./travis/build_docs.sh
if test "$TRAVIS_SECURE_ENV_VARS" = "true" -a "$TRAVIS_BRANCH" = "master"; then
    echo "publish website";
    ./travis/add_deploy_key.sh;
    ./travis/deploy_website.sh $TRAVIS_BUILD_DIR/docs/build /tmp;
fi
