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

scripts/cppcheck.sh
scripts/doxygen.sh

# Force sphinxcontrib-bibtex version to 0.4.2, because 1.0.0 requires
# sphinx >= 2.0 which is only available on Python >= 3.5, and this config
# has only 3.4
pip3 install --user sphinxcontrib-bibtex==0.4.2
pip3 install --user cpp-coveralls

./travis/docker.sh
