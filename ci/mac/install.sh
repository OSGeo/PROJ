#!/bin/bash

set -e

ccache -M 200M

CC="clang" CXX="clang++" CMAKE_BUILD_TYPE=RelWithDebInfo ./ci/install.sh
