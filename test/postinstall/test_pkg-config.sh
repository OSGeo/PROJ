#!/bin/sh

# Post-install tests with pkg-config and a Makefile
#
# First required argument is the installed prefix, which
# is used to set PKG_CONFIG_PATH and
# LD_LIBRARY_PATH/DYLD_LIBRARY_PATH for shared builds
# Second argument is either shared (default) or static
cd $(dirname $0)
. ./common.sh
main_setup $1 $2

echo "Running post-install tests with pkg-config (${BUILD_MODE})"

export PKG_CONFIG_PATH=${prefix}/lib/pkgconfig

make_all_test_clean(){
  set -e
  make -f makefile.mak clean
  make -f makefile.mak all
  make -f makefile.mak test
  make -f makefile.mak clean
}

echo "Testing C app"
cd c_app
make_all_test_clean
cd ..

echo "Testing C++ app"
cd cpp_app
make_all_test_clean
cd ..

echo "Finished running post-install tests with pkg-config (${BUILD_MODE})"
