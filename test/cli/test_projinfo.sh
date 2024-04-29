#!/bin/bash

# Test projinfo using specific sh commands
# See test_projinfo.yaml for general CLI testing.

set -e

TEST_CLI_DIR=$(dirname $0)
EXE=$1
if test -z "${EXE}"; then
    echo "Usage: ${0} <path to 'projinfo' program>"
    exit 1
fi
if test ! -x ${EXE}; then
    echo "*** ERROR: Can not find '${EXE}' program!"
    exit 1
fi

echo "============================================"
echo "Running ${0} using ${EXE}:"
echo "============================================"

OUT=$(basename $0 .sh)_out

echo "doing tests into file ${OUT}, please wait"
rm -f ${OUT}
#
echo "##############################################################" >> ${OUT}
echo "Testing PROJ_AUX_DB environment variable" >> ${OUT}
rm -f tmp_projinfo_aux.db
$EXE --dump-db-structure --output-id HOBU:XXXX EPSG:4326 | sqlite3 tmp_projinfo_aux.db
export PROJ_AUX_DB=tmp_projinfo_aux.db
$EXE HOBU:XXXX >>${OUT}
unset PROJ_AUX_DB
rm -f tmp_projinfo_aux.db
echo "" >>${OUT}


##############################################################################
# Done!
# do 'diff' with distribution results
echo "diff ${OUT} with ${OUT}.dist"
diff -u -b ${OUT} ${TEST_CLI_DIR}/${OUT}.dist
if [ $? -ne 0 ] ; then
	echo  ""
	echo "PROBLEMS HAVE OCCURRED"
	echo "test file ${OUT} saved"
	exit 100
else
	echo "TEST OK"
	echo "test file ${OUT} removed"
	rm -f ${OUT}
	exit 0
fi
