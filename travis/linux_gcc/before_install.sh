#!/bin/bash

set -e

./travis/before_install.sh

sudo apt-get update -qq
sudo apt-get install -y cppcheck
sudo apt-get install -qq lcov
sudo apt-get install -qq doxygen graphviz

scripts/cppcheck.sh

# Check that doxygen runs warning free
doxygen >docs_log.txt 2>&1
if grep -i warning docs_log.txt; then
    echo "Doxygen warnings found" && cat docs_log.txt && /bin/false;
else
    echo "No Doxygen warnings found";
fi

pip install --user sphinxcontrib-bibtex
pip install --user cpp-coveralls

./travis/docker.sh
