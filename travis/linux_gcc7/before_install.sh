#!/bin/bash

set -e

./travis/before_install_apt.sh
./travis/before_install_pip.sh

sudo apt-get install -qq \
        cppcheck \
        lcov \
        doxygen graphviz \
        openjdk-7-jdk \
        sqlite3 libsqlite3-dev

#scripts/cppcheck.sh
#scripts/doxygen.sh

#pip install --user sphinxcontrib-bibtex
#pip install --user cpp-coveralls

#./travis/docker.sh
