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
mkdir -p docs/build/html/
doxygen > docs/build/html/docs_log.txt 2>&1
if grep -i warning docs/build/html/docs_log.txt; then
    echo "Doxygen warnings found" && cat docs/build/html/docs_log.txt && /bin/false;
else
    echo "No Doxygen warnings found";
fi


# XML generation (for Breathe)
mkdir -p docs/build/tmp_breathe
python scripts/generate_breathe_friendly_general_doc.py
rm -rf docs/build/xml/

# Ugly hack to workaround a bug of Doxygen 1.8.17 that erroneously detect proj_network_get_header_value_cbk_type/ as a variable
sed "s/const char\* (\*proj_network_get_header_value_cbk_type/CONST_CHAR\* (\*proj_network_get_header_value_cbk_type/" < src/proj.h > docs/build/tmp_breathe/proj.h

(cat Doxyfile; printf "GENERATE_HTML=NO\nGENERATE_XML=YES\nINPUT= src/iso19111 include/proj docs/build/tmp_breathe/proj.h src/filemanager.cpp src/networkfilemanager.cpp docs/build/tmp_breathe/general_doc.dox.reworked.h") | doxygen -  > docs/build/tmp_breathe/docs_log.txt 2>&1
if grep -i warning docs/build/tmp_breathe/docs_log.txt; then
    echo "Doxygen warnings found" && cat docs/build/tmp_breathe/docs_log.txt && /bin/false;
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
sed "s/Convention/Convention_/g" < ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1io_1_1WKTFormatter.xml | sed "s/WKT2_2018/_WKT2_2018/g" | sed "s/WKT2_2019/_WKT2_2019/g" | sed "s/WKT2_2015/_WKT2_2015/g" | sed "s/WKT1_GDAL/_WKT1_GDAL/g" | sed "s/WKT1_ESRI/_WKT1_ESRI/g" > ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1io_1_1WKTFormatter.xml.tmp
mv ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1io_1_1WKTFormatter.xml.tmp  ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1io_1_1WKTFormatter.xml

# Ugly hack to workaround a bug of Doxygen 1.8.17 that erroneously detect proj_network_get_header_value_cbk_type/ as a variable
sed "s/CONST_CHAR/const char/"  < ${TOPDIR}/docs/build/xml/proj_8h.xml > ${TOPDIR}/docs/build/xml/proj_8h.xml.tmp
mv ${TOPDIR}/docs/build/xml/proj_8h.xml.tmp ${TOPDIR}/docs/build/xml/proj_8h.xml

# Hack for Breathe 4.17.0 issue that is confused by osgeo::proj::common::UnitOfMeasure::Type::NONE (enumeration value of Type) and osgeo::proj::common::UnitOfMeasure::NONE (member value), whereas 4.16.0 works fine.
# Filed as https://github.com/michaeljones/breathe/issues/518
sed "s/          <name>NONE<\/name>/          <name>Type::NONE<\/name>/"  < ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1common_1_1UnitOfMeasure.xml > ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1common_1_1UnitOfMeasure.xml.tmp
mv ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1common_1_1UnitOfMeasure.xml.tmp ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1common_1_1UnitOfMeasure.xml
sed 's/refid="classosgeo_1_1proj_1_1common_1_1UnitOfMeasure_1a400c71b5a6d7927e3e5850cee2b07d10ab50339a10e1de285ac99d4c3990b8693" kind="enumvalue"><name>NONE<\/name><\/member>/refid="classosgeo_1_1proj_1_1common_1_1UnitOfMeasure_1a400c71b5a6d7927e3e5850cee2b07d10ab50339a10e1de285ac99d4c3990b8693" kind="enumvalue"><name>Type::NONE<\/name><\/member>/' < ${TOPDIR}/docs/build/xml/index.xml > ${TOPDIR}/docs/build/xml/index.xml.tmp
mv ${TOPDIR}/docs/build/xml/index.xml.tmp ${TOPDIR}/docs/build/xml/index.xml

sed "s/noexceptoverride/noexcept override/" < ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1util_1_1Exception.xml > ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1util_1_1Exception.xml.tmp
mv ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1util_1_1Exception.xml.tmp ${TOPDIR}/docs/build/xml/classosgeo_1_1proj_1_1util_1_1Exception.xml

popd > /dev/null || exit

