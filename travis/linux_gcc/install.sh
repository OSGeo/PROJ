#!/bin/bash

set -e

export CCACHE_CPP2=yes

CC="ccache gcc" CFLAGS="-std=c89 -Werror" ./travis/install.sh
