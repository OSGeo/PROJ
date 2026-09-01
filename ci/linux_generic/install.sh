#!/bin/bash

set -e

# -fno-use-cxa-atexit is needed to build with -coverage
./ci/install.sh
