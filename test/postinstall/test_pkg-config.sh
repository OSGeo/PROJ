#!/bin/sh

# Post-install tests with pkg-config
#
# First required argument is the installed prefix, which
# is used to set PKG_CONFIG_PATH and LD_LIBRARY_PATH

set -e

echo "Running post-install tests with pkg-config"

prefix=$1
if [ -z "$prefix" ]; then
    echo "First positional argument to the the installed prefix is required"
    exit 1
fi

export PKG_CONFIG_PATH=$prefix/lib/pkgconfig
export LD_LIBRARY_PATH=$prefix/lib

echo "PKG_CONFIG_PATH=$PKG_CONFIG_PATH"

PKG_CONFIG_MODVERSION=$(pkg-config proj --modversion)
echo "pkg-config proj --modversion: $PKG_CONFIG_MODVERSION"
PKG_CONFIG_DATADIR=$(pkg-config proj --variable=datadir)
echo "pkg-config proj --variable=datadir: $PKG_CONFIG_DATADIR"

UNAME=$(uname)
case $UNAME in
  Darwin*)
    alias ldd="otool -L" ;;
  Linux*)
    ;;
  *)
    echo "no ldd equivalent found for UNAME=$UNAME"
    exit 1 ;;
esac

cd $(dirname $0)

PROGRAM=testappprojinfo
cd $PROGRAM
make

# Run tests from shell, count any errors
ERRORS=0

LDD_OUTPUT=$(ldd ./$PROGRAM | grep proj)
LDD_SUBSTR=$LD_LIBRARY_PATH/libproj.
printf "Testing expected ldd output ... "
case "$LDD_OUTPUT" in
  *$LDD_SUBSTR*)
    echo "passed" ;;
  *)
    ERRORS=$(($ERRORS + 1))
    echo "failed: ldd output '$LDD_OUTPUT' does not contain '$LDD_SUBSTR'" ;;
esac

SEARCHPATH_OUTPUT=$(./$PROGRAM -s)
printf "Testing expected searchpath/datadir ... "
case "$SEARCHPATH_OUTPUT" in
  *$PKG_CONFIG_DATADIR*)
    echo "passed" ;;
  *)
    ERRORS=$(($ERRORS + 1))
    echo "failed: searchpath '$SEARCHPATH_OUTPUT' does not contain '$PKG_CONFIG_DATADIR'" ;;
esac

VERSION_OUTPUT=$(./$PROGRAM -v)
printf "Testing expected version ... "
case "$VERSION_OUTPUT" in
  $PKG_CONFIG_MODVERSION)
    echo "passed" ;;
  *)
    ERRORS=$(($ERRORS + 1))
    echo "failed: '$VERSION_OUTPUT' != '$PKG_CONFIG_MODVERSION'" ;;
esac

make clean

cd ..

exit $ERRORS
