#!/bin/sh
#

#DOCSDIR="/home/docs/checkouts/readthedocs.org/user_builds/$READTHEDOCS_PROJECT/checkouts/$READTHEDOCS_VERSION/docs"

CONTENTLOCATION="../../artifacts/${READTHEDOCS_VERSION}/sphinx"

find ../../artifacts/${READTHEDOCS_VERSION}/sphinx/


# content loation from https://github.com/readthedocs/readthedocs.org/issues/9172

sed "s/io::Convention_/io::Convention/g" < ${CONTENTLOCATION}/development/reference/cpp/io.html | sed "s/>Convention_/>Convention/g" | sed "s/_WKT2/WKT2/g" | sed "s/_WKT1/WKT1/g" > ${CONTENTLOCATION}/development/reference/cpp/io.html.tmp
mv ${CONTENTLOCATION}/development/reference/cpp/io.html.tmp ${CONTENTLOCATION}/development/reference/cpp/io.html


cp -r schemas ${CONTENTLOCATION}/schemas
cp ./data/triangulation.schema.json ${CONTENTLOCATION}/schemas


