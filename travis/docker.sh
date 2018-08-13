#!/bin/bash

# proj-docs image has all of the Sphinx
# dependencies need to build PROJ's docs
# it is build from docker/docbuild/Dockerfile
# and is manually pushed as needed to DockerHub

docker pull osgeo/proj-docs


