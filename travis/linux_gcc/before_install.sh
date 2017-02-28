#!/bin/bash

sudo apt-get install -y cppcheck

cppcheck --inline-suppr --template='{file}:{line},{severity},{id},{message}' --enable=all --inconclusive --std=posix -DPJ_SELFTEST=1 src/*.c 2>/tmp/cppcheck.txt

grep "error," /tmp/cppcheck.txt
if [[ $? -eq 0 ]] ; then
    echo "cppcheck failed"
    exit 1
fi

set -e

./travis/before_install.sh
