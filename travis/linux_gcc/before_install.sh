#!/bin/bash

set -e
sudo apt-get update -qq
sudo apt-get install -qq python3-pip

./travis/before_install.sh

sudo apt-get install -y cppcheck
sudo apt-get install -qq lcov
sudo apt-get install -qq doxygen graphviz
sudo apt-get install -qq sqlite3 libsqlite3-dev
sudo apt-get install -qq openjdk-7-jdk

scripts/cppcheck.sh
scripts/doxygen.sh

pip3 install --user sphinxcontrib-bibtex
pip3 install --user cpp-coveralls
pip3 install --user sphinxcontrib-bibtex==0.4.2

./travis/docker.sh
