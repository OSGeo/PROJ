#!/bin/bash

set -e

./travis/before_install.sh

sudo apt-get update -qq
sudo apt-get install -y cppcheck
sudo apt-get install -qq lcov

scripts/cppcheck.sh

pip install --user sphinxcontrib-bibtex
pip install --user cpp-coveralls

./travis/docker.sh
