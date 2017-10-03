#!/bin/bash

set -e

export CCACHE_CPP2=yes

CC="ccache clang" CFLAGS="-g -Wall -Wextra -Werror -Wunused-parameter -Wmissing-prototypes -Wmissing-declarations -Wformat -Werror=format-security -Wshadow -Wfloat-conversion -fsanitize=address -O2" ./travis/install.sh
