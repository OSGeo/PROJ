#!/bin/bash

./travis/before_install.sh

sudo apt-get install -y cppcheck

cppcheck --inline-suppr --template='{file}:{line},{severity},{id},{message}' --enable=all --inconclusive --std=posix src/*.c 2>/tmp/cppcheck.txt

grep "error," /tmp/cppcheck.txt
if [[ $? -eq 0 ]] ; then
    echo "cppcheck failed"
    exit 1
fi

set -e

pip install --user cpp-coveralls
./travis/docker.sh

