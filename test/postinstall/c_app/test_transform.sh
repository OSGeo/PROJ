#!/bin/sh
. ../common.sh

PROGRAM_TRANSFORM="$(./c_app -t)"

test_transform "${PROGRAM_TRANSFORM}"
