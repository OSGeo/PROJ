#!/bin/bash

set -eu

SCRIPT_DIR=$(dirname "$0")
case $SCRIPT_DIR in
    "/"*)
        ;;
    ".")
        SCRIPT_DIR=$(pwd)
        ;;
    *)
        SCRIPT_DIR=$(pwd)/$(dirname "$0")
        ;;
esac

TOPDIR="$SCRIPT_DIR/.."

pushd "${TOPDIR}" > /dev/null || exit

# Check that doxygen runs warning free
doxygen > /tmp/docs_log.txt 2>&1
if grep -i warning /tmp/docs_log.txt; then
    echo "Doxygen warnings found" && cat /tmp/docs_log.txt && /bin/false;
else
    echo "No Doxygen warnings found";
fi

for i in ${TOPDIR}/xml/*; do

# Fix Breathe error on Doxygen XML
# Type must be either just a name or a typedef-like declaration.
#   If just a name:
#     Invalid definition: Expected end of definition. [error at 32]
#       osgeo::proj::common::MeasurePtr = typedef std::shared_ptr<Measure>
    sed -i "s/ = typedef /=/g" $i;
done

# There is a confusion for Breathe between PROJStringFormatter::Convention and WKTFormatter:Convention
sed -i "s/Convention/Convention_/g" ${TOPDIR}/xml/classosgeo_1_1proj_1_1io_1_1WKTFormatter.xml

popd > /dev/null || exit

