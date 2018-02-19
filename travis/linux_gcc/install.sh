#!/bin/bash

set -e

export CCACHE_CPP2=yes

#  -Wfloat-conversion not available for gcc 4.8
CC="ccache gcc" CFLAGS="-std=c89 -g -Wall -Wextra -Werror -Wunused-parameter -Wmissing-prototypes -Wmissing-declarations -Wformat -Werror=format-security -Wshadow -O2" ./travis/install.sh
