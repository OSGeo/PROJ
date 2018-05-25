#!/bin/sh
set -eu

# To filter only on c++ stuff:
# scripts/gen_html_coverage.sh -ext "*.cpp,*.hh"

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

FILTER=""
if test $# -ge 1; then
    if test "$1" = "--help"; then
        echo "Usage: gen_html_coverage.sh [--help] [-ext \"ext1,...\"]"
        exit
    fi

    if test "$1" = "-ext"; then
        FILTER="$2"
        shift
        shift
    fi

    if test $# -ge 1; then
        echo "Invalid option: $1"
        echo "Usage: gen_html_coverage.sh [--help] [-ext \"ext1,...\"]"
        exit
    fi
fi

rm -rf coverage_html
lcov --directory src --directory include --capture --output-file proj.info
"$SCRIPT_DIR"/filter_lcov_info.py "$FILTER" < proj.info > proj.info.filtered
mv proj.info.filtered proj.info
genhtml -o ./coverage_html --ignore-errors source --num-spaces 2 proj.info 
