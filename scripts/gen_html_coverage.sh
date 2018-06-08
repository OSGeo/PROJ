#!/bin/sh
set -eu
rm -rf coverage_html
lcov --directory src --capture --output-file proj.info
genhtml -o ./coverage_html --ignore-errors source --num-spaces 2 proj.info 
