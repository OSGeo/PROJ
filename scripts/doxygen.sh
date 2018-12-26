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

# HTML generation
# Check that doxygen runs warning free
rm -rf docs/build/html/
mkdir -p docs/build/
doxygen > /tmp/docs_log.txt 2>&1
if grep -i warning /tmp/docs_log.txt; then
    echo "Doxygen warnings found" && cat /tmp/docs_log.txt && /bin/false;
else
    echo "No Doxygen warnings found";
fi


# XML generation (for Breathe)
mkdir -p docs/build/tmp_breathe
python scripts/generate_breathe_friendly_general_doc.py
rm -rf docs/build/xml/
(cat Doxyfile; printf "GENERATE_HTML=NO\nGENERATE_XML=YES\nINPUT= src/iso19111 include/proj src/proj.h docs/build/tmp_breathe/general_doc.dox.reworked.h") | doxygen -  > /tmp/docs_log.txt 2>&1
if grep -i warning /tmp/docs_log.txt; then
    echo "Doxygen warnings found" && cat /tmp/docs_log.txt && /bin/false;
else
    echo "No Doxygen warnings found";
fi

for i in ${TOPDIR}/docs/build/xml/*; do

# Fix Breathe error on Doxygen XML
# Type must be either just a name or a typedef-like declaration.
#   If just a name:
#     Invalid definition: Expected end of definition. [error at 32]
#       osgeo::proj::common::MeasurePtr = typedef std::shared_ptr<Measure>
    sed "s/ = typedef /=/g" < $i > $i.tmp;
    mv $i.tmp $i
done

# There is a confusion for Breathe between PROJStringFormatter::Convention and WKTFormatter:Convention
sed "s/Convention/Convention_/g" < ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1io_1_1WKTFormatter.xml | sed "s/WKT2_2018/_WKT2_2018/g" | sed "s/WKT2_2015/_WKT2_2015/g" | sed "s/WKT1_GDAL/_WKT1_GDAL/g" | sed "s/WKT1_ESRI/_WKT1_ESRI/g" > ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1io_1_1WKTFormatter.xml.tmp
mv ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1io_1_1WKTFormatter.xml.tmp  ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1io_1_1WKTFormatter.xml

popd > /dev/null || exit

