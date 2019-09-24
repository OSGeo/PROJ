#!/bin/bash

set -e

./travis/before_install_apt.sh
./travis/before_install_pip.sh

sudo apt-get install -qq \
        lcov \
        doxygen graphviz \
        sqlite3 libsqlite3-dev

# Install Cppcheck to maintain version 1.61
LIBTINYXML=libtinyxml2-0.0.0_0~git20120518.1.a2ae54e-1_amd64.deb
CPPCHECK=cppcheck_1.61-1_amd64.deb
wget -q http://security.ubuntu.com/ubuntu/pool/universe/t/tinyxml2/$LIBTINYXML
wget -q http://security.ubuntu.com/ubuntu/pool/universe/c/cppcheck/$CPPCHECK
sudo dpkg -i $LIBTINYXML
sudo dpkg -i $CPPCHECK

scripts/cppcheck.sh
scripts/doxygen.sh

pip3 install --user sphinxcontrib-bibtex
pip3 install --user cpp-coveralls

./travis/docker.sh
