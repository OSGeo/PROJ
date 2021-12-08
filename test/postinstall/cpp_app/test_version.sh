#!/bin/sh
. ../common.sh

PROGRAM_VERSION=$(./cpp_app -v)
EXPECTED_VERSION=$(pkg-config proj --modversion)

test_version ${PROGRAM_VERSION} ${EXPECTED_VERSION}
