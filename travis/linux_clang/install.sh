#!/bin/bash

set -e

export CCACHE_CPP2=yes

CC="ccache clang" CFLAGS="-std=c89 -Werror -fsanitize=address" ./travis/install.sh
