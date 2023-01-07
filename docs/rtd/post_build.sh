#!/bin/sh
#

BUILDDIR="/home/docs/checkouts/readthedocs.org/user_builds/$READTHEDOCS_PROJECT/checkouts/$READTHEDOCS_VERSION"

pwd
#/home/docs/checkouts/readthedocs.org/user_builds/osgeo-proj/checkouts/3538
#/home/docs/checkouts/readthedocs.org/user_builds/osgeo-proj/checkouts/3538

echo "listing BUILDDIR: $BUILDDIR"


ls $BUILDDIR

find .

echo "listing BUILDDIR: $BUILDDIR/docs"

ls $BUILDDIR/docs

echo "listing BUILDDIR: $BUILDDIR/docs/build"

ls $BUILDDIR/docs/build

echo "listing BUILDDIR: $BUILDDIR/docs/build"

ls $BUILDDIR/

BUILDDIR="/home/docs/checkouts/readthedocs.org/user_builds/$READTHEDOCS_PROJECT/$READTHEDOCS_VERSION/docs/build"

ls $BUILDDIR

echo "listing BUILDDIR: $BUILDDIR/docs/build"

ls $BUILDDIR/docs/build
sed "s/io::Convention_/io::Convention/g" < ${BUILDDIR}/html/development/reference/cpp/io.html | sed "s/>Convention_/>Convention/g" | sed "s/_WKT2/WKT2/g" | sed "s/_WKT1/WKT1/g" > ${BUILDDIR}/html/development/reference/cpp/io.html.tmp
mv ${BUILDDIR}/html/development/reference/cpp/io.html.tmp ${BUILDDIR}/html/development/reference/cpp/io.html
cp -r ../schemas ${BUILDDIR}/html/schemas
cp ../data/triangulation.schema.json ${BUILDDIR}/html/schemas


