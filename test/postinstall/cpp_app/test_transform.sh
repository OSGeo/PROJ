#!/bin/sh
. ../common.sh

PROGRAM_TRANSFORM="$(./cpp_app -t)"

test_transform "${PROGRAM_TRANSFORM}"
