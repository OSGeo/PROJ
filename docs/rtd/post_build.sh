#!/bin/sh
#

mkdir build
cd build

BUILDDIR="/home/docs/checkouts/readthedocs.org/user_builds/$READTHEDOCS_PROJECT/docs/"

echo "updating CPP files and copying schemas for BUILDDIR: $BUILDDIR"


sed "s/io::Convention_/io::Convention/g" < ${BUILDDIR}/html/development/reference/cpp/io.html | sed "s/>Convention_/>Convention/g" | sed "s/_WKT2/WKT2/g" | sed "s/_WKT1/WKT1/g" > ${BUILDDIR}/html/development/reference/cpp/io.html.tmp
mv ${BUILDDIR}/html/development/reference/cpp/io.html.tmp ${BUILDDIR}/html/development/reference/cpp/io.html
cp -r ../schemas ${BUILDDIR}/html/schemas
cp ../data/triangulation.schema.json ${BUILDDIR}/html/schemas


