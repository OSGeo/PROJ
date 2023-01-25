#!/bin/sh
set -e

# https://github.com/readthedocs/readthedocs.org/pull/9888
READTHEDOCS_OUTPUT="_readthedocs"
find ${READTHEDOCS_OUTPUT} -type d
READTHEDOCS_OUTPUT_HTML="${READTHEDOCS_OUTPUT}/html"

# Undoes the hack done in scripts/doxygen.sh
sed "s/io::Convention_/io::Convention/g" < ${READTHEDOCS_OUTPUT_HTML}/development/reference/cpp/io.html | sed "s/>Convention_/>Convention/g" | sed "s/_WKT2/WKT2/g" | sed "s/_WKT1/WKT1/g" > ${READTHEDOCS_OUTPUT_HTML}/development/reference/cpp/io.html.tmp
mv ${READTHEDOCS_OUTPUT_HTML}/development/reference/cpp/io.html.tmp ${READTHEDOCS_OUTPUT_HTML}/development/reference/cpp/io.html

# Undoes the hacks of scripts/generate_breathe_friendly_general_doc.py
sed "s/<em class=\"property\">namespace <\/em>//g" < ${READTHEDOCS_OUTPUT_HTML}/development/reference/cpp/cpp_general.html > ${READTHEDOCS_OUTPUT_HTML}/development/reference/cpp/cpp_general.html.tmp
mv ${READTHEDOCS_OUTPUT_HTML}/development/reference/cpp/cpp_general.html.tmp ${READTHEDOCS_OUTPUT_HTML}/development/reference/cpp/cpp_general.html

# Copy JSON schemas for public consumption
READTHEDOCS_OUTPUT_SCHEMAS="${READTHEDOCS_OUTPUT_HTML}/schemas"
cp -r schemas ${READTHEDOCS_OUTPUT_SCHEMAS}
