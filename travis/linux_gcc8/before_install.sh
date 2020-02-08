#!/bin/bash

set -e

./travis/before_install_apt.sh
./travis/before_install_pip.sh

sudo apt-get install -qq \
        lcov \
        doxygen graphviz \
        sqlite3 libsqlite3-dev:$ARCH \
        libtiff-dev:$ARCH libcurl4-openssl-dev:$ARCH

#scripts/cppcheck.sh
#scripts/doxygen.sh

#pip install --user sphinxcontrib-bibtex
#pip install --user cpp-coveralls

#./travis/docker.sh
