#!/bin/bash

# Script to do some locale testing.
# See test_cs2cs_various.yaml for general CLI testing.

set -e

TEST_CLI_DIR=$(dirname $0)
EXE=$1
if test -z "${EXE}"; then
    echo "Usage: ${0} <path to 'cs2cs' program>"
    exit 1
fi
if test ! -x ${EXE}; then
    echo "*** ERROR: Can not find '${EXE}' program!"
    exit 1
fi

# Would be great to have a universale way of selecting a locale with
# a decimal separator that is not '.'
if command locale >/dev/null 2>/dev/null; then
    if test `locale -a | grep fr_FR.utf8`; then
        echo "Using locale with comma as decimal separator"
        export LC_ALL=fr_FR.UTF-8
        export PROJ_USE_ENV_LOCALE=1
    fi
fi

echo "============================================"
echo "Running ${0} using ${EXE}:"
echo "============================================"

OUT=$(basename $0 .sh)_out

#
echo "doing tests into file ${OUT}, please wait"
rm -f ${OUT}
#
echo "##############################################################" >> ${OUT}
echo Between two 3parameter approximations on same ellipsoid >> ${OUT}
#
$EXE +proj=latlong +ellps=bessel +towgs84=5,0,0 \
 +to +proj=latlong +ellps=bessel +towgs84=1,0,0 \
 -E >>${OUT} <<EOF
0d00'00.000"W 0d00'00.000"N 0.0
79d00'00.000"W 45d00'00.000"N 0.0
EOF
echo "##############################################################" >> ${OUT}
echo Test input in grad >> ${OUT}
#
$EXE EPSG:4807 EPSG:27572 -E >>${OUT} <<EOF
64.44444444 2.9586342556
EOF
echo "##############################################################" >> ${OUT}
echo Test geocentric x/y/z generation. >> ${OUT}
#
$EXE +proj=latlong +datum=WGS84  \
 +to +proj=geocent +datum=WGS84  \
 -E >>${OUT} <<EOF
0d00'00.001"W 0d00'00.001"N 0.0
0d00'00.001"W 0d00'00.001"N 10.0
79d00'00.000"W 45d00'00.000"N 0.0
45d00'00.000"W 89d59'59.990"N 0.0
EOF
#
echo "##############################################################" >> ${OUT}
echo Test geocentric x/y/z consumption. >> ${OUT}
#
$EXE +proj=geocent +datum=WGS84  \
 +to +proj=latlong +datum=WGS84  \
 -E >>${OUT} <<EOF
6378137.00      -0.00 0.00
6378147.00      -0.00 0.00
861996.98       -4434590.01 4487348.41
0.00    -0.00 6356752.31
EOF
#
echo "#############################################################" >> ${OUT}
echo Test conversion from geodetic latlong to geocentric latlong >> ${OUT}
#
$EXE +proj=latlong +datum=WGS84 \
 +to +proj=latlong +datum=WGS84 +geoc  \
 -E >>${OUT} <<EOF
0d00'00.000"W 0d00'00.000"N 0.0
79d00'00.000"W 45d00'00.000"N 0.0
12d00'00.000"W 45d00'00.000"N 0.0
0d00'00.000"W 90d00'00.000"N 0.0
EOF
#
echo "#############################################################" >> ${OUT}
echo Test conversion from geocentric latlong to geodetic latlong >> ${OUT}
#
$EXE +proj=latlong +datum=WGS84 +geoc \
 +to +proj=latlong +datum=WGS84  \
 -E >>${OUT} <<EOF
0d00'00.000"W 0d00'00.000"N 0.0
79d00'00.000"W 44d48'27.276"N 0.000
12d00'00.000"W 44d48'27.276"N 0.0
0d00'00.000"W 90d00'00.000"N 0.0
EOF


# Done!
# do 'diff' with distribution results
echo "diff ${OUT} with ${OUT}.dist"
diff -u -b ${OUT} ${TEST_CLI_DIR}/${OUT}.dist
if [ $? -ne 0 ] ; then
	echo  ""
	echo "PROBLEMS HAVE OCCURRED"
	echo "test file ${OUT} saved"
    echo
    echo "----------------------------------------------------------"
	echo "${OUT}"
    echo "----------------------------------------------------------"
    cat ${OUT}
    echo "----------------------------------------------------------"
	exit 100
else
	echo "TEST OK"
	echo "test file ${OUT} removed"
    echo
	rm -f ${OUT}
	exit 0
fi
