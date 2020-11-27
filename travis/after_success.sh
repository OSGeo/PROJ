#!/bin/bash

set -e

echo "$TRAVIS_SECURE_ENV_VARS"
./travis/build_docs.sh
# Only build and publish docs when changes are made on most recent maintenance branch
if test "$TRAVIS_SECURE_ENV_VARS" = "true" -a "$TRAVIS_BRANCH" = "7.2"; then
    echo "publish website";
    ./travis/add_deploy_key.sh;
    ./travis/deploy_website.sh $TRAVIS_BUILD_DIR/docs/build /tmp;

    # Disabled: see discussion of https://github.com/OSGeo/proj.4/pull/1035
    # if test "$BUILD_NAME" = "linux_gcc"; then
    #     ./scripts/gen_html_coverage.sh
    #     ./travis/deploy_html_coverage.sh
    # fi
fi
