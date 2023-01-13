#!/bin/sh
set -e

# content location from https://github.com/readthedocs/readthedocs.org/issues/9172
CONTENTLOCATION="../../artifacts/${READTHEDOCS_VERSION}/sphinx"

find ${CONTENTLOCATION}

# Undoes the hack done in scripts/doxygen.sh
sed "s/io::Convention_/io::Convention/g" < ${CONTENTLOCATION}/development/reference/cpp/io.html | sed "s/>Convention_/>Convention/g" | sed "s/_WKT2/WKT2/g" | sed "s/_WKT1/WKT1/g" > ${CONTENTLOCATION}/development/reference/cpp/io.html.tmp
mv ${CONTENTLOCATION}/development/reference/cpp/io.html.tmp ${CONTENTLOCATION}/development/reference/cpp/io.html

# Copy JSON schemata for public consumption
cp -r schemas ${CONTENTLOCATION}/schemas
cp ./data/triangulation.schema.json ${CONTENTLOCATION}/schemas
