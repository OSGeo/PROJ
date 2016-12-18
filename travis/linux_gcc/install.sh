#!/bin/bash

set -e

export CCACHE_CPP2=yes

CC="ccache gcc" ./travis/install.sh
