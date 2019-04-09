#!/bin/bash

set -e

# prepare build files
./autogen.sh

CXXFLAGS="-std=c++11" ./clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04/bin/scan-build -o scanbuildoutput -plist -v ./configure
./clang+llvm-6.0.0-x86_64-linux-gnu-ubuntu-14.04/bin/scan-build -o scanbuildoutput -plist -v make -j2

if grep -r "\.c" scanbuildoutput | grep "<string>" | grep -v -e "<key>" -e _generated_parser; then echo "error" && /bin/false; else echo "ok"; fi 
