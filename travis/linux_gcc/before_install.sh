#!/bin/bash

set -e

./travis/before_install_apt.sh
./travis/before_install_pip.sh

sudo apt-get install -qq \
        lcov \
        doxygen graphviz \
        sqlite3 libsqlite3-dev \
        libtiff-dev libcurl4-openssl-dev \
        cppcheck

scripts/cppcheck.sh
scripts/doxygen.sh

pip3 install --user sphinxcontrib-bibtex
pip3 install --user cpp-coveralls

./travis/docker.sh
