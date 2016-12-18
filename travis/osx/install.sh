#!/bin/bash

set -e

export CCACHE_CPP2=yes

CC="ccache clang" ./travis/install.sh
