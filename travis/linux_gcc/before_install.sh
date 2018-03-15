#!/bin/bash

./travis/before_install.sh

sudo apt-get install -y cppcheck

set -e

scripts/cppcheck.sh

pip install --user cpp-coveralls
./travis/docker.sh

