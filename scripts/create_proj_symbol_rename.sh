#!/bin/bash

# Script to extract exported symbols and rename them
# to avoid clashing with other older libproj
#
# This is done in provision for the integration of PROJ master with GDAL. GDAL is
# a complex library that links with many libraries, a number of them linking with
# PROJ. However in the continuous integration setups used by GDAL, it would be
# impractical to recompile all those libraries that use the system libproj (4.X or 5.X)
# that comes with the distribution. Linking GDAL directly against PROJ master
# and this system libproj with potential different ABI is prone to clash/crash
# (we experimented that painfully in GDAL with its internal libtiff 4.X copy whereas
# systems shipped with libtiff 3.X)
# Hence this solution to rename the symbols of PROJ master so that PROJ master
# can be used by GDAL without conflicting with the system PROJ.
# The renaming only happens if -DPROJ_RENAME_SYMBOLS is passed in CFLAGS and CXXFLAGS.
#
# Note: we potentially should do the same for C++ symbols, but that is not needed
# for now, and we would just set the NS_PROJ existing macro to change the C++ base
# namespace which defaults to osgeo::proj currently.

# To be run from a 'build' subdir after running configure

PROJ_ROOT=$(cd $(dirname ${BASH_SOURCE[0]})/..; pwd)

rm -rf tmp_alias
mkdir tmp_alias
for i in ${PROJ_ROOT}/src/*.c; do
    echo $i
    (cd tmp_alias; gcc -fvisibility=hidden ${i} -I../src -I${PROJ_ROOT}/src -I${PROJ_ROOT}/include -fPIC -c)
done
for i in ${PROJ_ROOT}/src/*.cpp ${PROJ_ROOT}/src/iso19111/*.cpp; do
    echo $i
    (cd tmp_alias; g++ -fvisibility=hidden -std=c++11 ${i} -I../src -I${PROJ_ROOT}/src -I${PROJ_ROOT}/include -fPIC -c)
done

(cd tmp_alias; g++ -fvisibility=hidden -std=c++11 *.o -shared -o libproj.so)

OUT_FILE=proj_symbol_rename.h

rm $OUT_FILE 2>/dev/null

echo "/* This is a generated file by create_proj_symbol_rename.sh. *DO NOT EDIT MANUALLY !* */" >> $OUT_FILE
echo "#ifndef PROJ_SYMBOL_RENAME_H" >> $OUT_FILE
echo "#define PROJ_SYMBOL_RENAME_H" >> $OUT_FILE

symbol_list=$(objdump -t tmp_alias/libproj.so   | grep " g     " | grep .text | awk '{print $6}' | grep -v -e _Z | sort)
for symbol in $symbol_list
do
    echo "#define $symbol internal_$symbol" >> $OUT_FILE
done

rodata_symbol_list=$(objdump -t tmp_alias/libproj.so | grep " g     O \\.rodata" |  awk '{print $6}')
for symbol in $rodata_symbol_list
do
    echo "#define $symbol internal_$symbol" >> $OUT_FILE
done

#data_symbol_list=$(objdump -t tmp_alias/libproj.so | grep "\\.data"  | grep -v -e __dso_handle -e __TMC_END__ | awk '{print $6}' | grep -v "\\."| grep -v _Z)
#for symbol in $data_symbol_list
#do
#    echo "#define $symbol internal_$symbol" >> $OUT_FILE
#done

bss_symbol_list=$(objdump -t tmp_alias/libproj.so  | grep "g     O \\.bss" | awk '{print $6}' | grep -v "\\."| grep -v _Z)
for symbol in $bss_symbol_list
do
    echo "#define $symbol internal_$symbol" >> $OUT_FILE
done

echo "#endif /* PROJ_SYMBOL_RENAME_H */" >> $OUT_FILE
