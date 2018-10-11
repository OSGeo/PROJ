#!/bin/bash

set -e

if test "$TRAVIS_BUILD_DIR" = ""; then
    TRAVIS_BUILD_DIR=$PWD
fi

cd docs
echo "building docs for $TRAVIS_BUILD_DIR/docs"
docker run --user $(id -u):$(id -g) -v $TRAVIS_BUILD_DIR:/data -w /data/docs osgeo/proj-docs make html
docker run --user $(id -u):$(id -g) -v $TRAVIS_BUILD_DIR:/data -w /data/docs osgeo/proj-docs make latexpdf


