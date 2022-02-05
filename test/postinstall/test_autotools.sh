#!/bin/sh

# Post-install tests with autotools/pkg-config
#
# First required argument is the installed prefix, which
# is used to set PKG_CONFIG_PATH and rpath for shared.
# Second argument is either shared (default) or static
cd $(dirname $0)
. ./common.sh
main_setup $1 $2

echo "Running post-install tests with autotools/pkg-config (${BUILD_MODE})"

if [ ${BUILD_MODE} = shared ]; then
  export PKG_CONFIG="${PKG_CONFIG:-pkg-config}"
  ENABLE_STATIC_PROJ=no
else
  export PKG_CONFIG="${PKG_CONFIG:-pkg-config} --static"
  ENABLE_STATIC_PROJ=yes
fi
echo ,${PKG_CONFIG},

export PKG_CONFIG_PATH=${prefix}/lib/pkgconfig

if [ ${BUILD_MODE} = shared ]; then
  export LDFLAGS="${LDFLAGS} -Wl,-rpath,$(pkg-config proj --variable=libdir)"
fi

autogen_configure_check_clean(){
  set -e
  aclocal
  automake --add-missing --copy --foreign
  autoconf
  ./configure --enable-static-proj=${ENABLE_STATIC_PROJ}
  make clean
  make
  set +e
  make check
  EXIT=$?
  if [ ${EXIT} -ne 0 ]; then
    cat test-suite.log
    exit ${EXIT}
  fi
  make distclean
}

echo "Testing C app"
cd c_app
autogen_configure_check_clean
cd ..

echo "Testing C++ app"
cd cpp_app
autogen_configure_check_clean
cd ..

echo "Finished running post-install tests with autotools/pkg-config (${BUILD_MODE})"
