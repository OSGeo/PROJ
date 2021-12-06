#!/bin/sh
. ../common.sh

PROGRAM_SEARCHPATH="$(./c_app -s)"
EXPECTED_SEARCHPATH="$(pkg-config proj --variable=datadir)"

test_searchpath "${PROGRAM_SEARCHPATH}" "${EXPECTED_SEARCHPATH}"
