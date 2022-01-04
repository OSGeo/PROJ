#!/bin/bash

set -e

sudo apt update

DEBIAN_FRONTEND=noninteractive sudo apt-get install -y --no-install-recommends \
    g++ cmake make sqlite3 libsqlite3-dev libtiff-dev libcurl4-openssl-dev jq

CLANG_LLVM=clang+llvm-9.0.0-x86_64-linux-gnu-ubuntu-18.04
wget -nv https://releases.llvm.org/9.0.0/$CLANG_LLVM.tar.xz
tar xJf $CLANG_LLVM.tar.xz
mv $CLANG_LLVM clang+llvm-9

NPROC=$(nproc)
echo "NPROC=${NPROC}"
export MAKEFLAGS="-j ${NPROC}"

export PATH=$PWD/clang+llvm-9/bin:$PATH

mkdir csa_build
cd csa_build

scan-build -o scanbuildoutput -plist -v cmake ..

scan-build -o scanbuildoutput -sarif -v -enable-checker alpha.unix.cstring.OutOfBounds,alpha.unix.cstring.BufferOverlap,optin.cplusplus.VirtualCall,optin.cplusplus.UninitializedObject make

rm -f filtered_scanbuild.txt
files=$(find scanbuildoutput -name "*.sarif")
for f in $files; do
    jq '.runs[].results[] | (if .locations[].physicalLocation.fileLocation.uri | (contains("_generated_parser") ) then empty else { "uri": .locations[].physicalLocation.fileLocation.uri, "msg": .message.text, "location": .codeFlows[-1].threadFlows[-1].locations[-1] } end)' < $f > tmp.txt
    if [ -s tmp.txt ]; then
        echo "Errors from $f: "
        cat $f
        echo ""
        cat tmp.txt >> filtered_scanbuild.txt
    fi
    rm -f tmp.txt
done
if [ -s filtered_scanbuild.txt ]; then
    echo ""
    echo ""
    echo "========================"
    echo "Summary of errors found:"
    cat filtered_scanbuild.txt
    /bin/false
fi
