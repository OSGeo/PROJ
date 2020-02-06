#!/bin/bash

set -e

# prepare build files
./autogen.sh

export PATH=$PWD/clang+llvm-9/bin:$PATH
CXXFLAGS="-std=c++11" scan-build -o scanbuildoutput -plist -v ./configure
rm -rf scanbuildoutput
TOPDIR=$PWD
scan-build -o $TOPDIR/scanbuildoutput -sarif -v -enable-checker alpha.unix.cstring.OutOfBounds,alpha.unix.cstring.BufferOverlap,optin.cplusplus.VirtualCall,optin.cplusplus.UninitializedObject make -j2

sudo apt-get install jq

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
done
if [ -s filtered_scanbuild.txt ]; then
    echo ""
    echo ""
    echo "========================"
    echo "Summary of errors found:"
    cat filtered_scanbuild.txt
    /bin/false
fi
