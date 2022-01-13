#!/bin/sh

# Common shell functions for post-install tests

main_setup(){
  # usage: main_setup $1 $2
  export prefix=$1
  if [ -z "${prefix}" ]; then
    echo "First positional argument to the the installed prefix is required"
    exit 1
  fi
  case $2 in
    "" | shared) export BUILD_MODE=shared ;;
         static) export BUILD_MODE=static ;;
    *)
      echo "Second argument must be either shared (default) or static"
      exit 1 ;;
  esac
}

test_libpath(){
  # usage: test_libpath ${PROGRAM} ${LIBPATH} ${LIBNAME}
  # use optional 'BUILD_MODE=static' to pass if match is not found
  UNAME=$(uname)
  case ${UNAME} in
    Darwin*)
      USE_OTOOL=yes
      EXPECTED_SUBSTR=$2
      ;;
    MINGW* | MSYS*)
      EXPECTED_SUBSTR=$(cygpath -u "$2/$3" | sed 's/\/lib\//\/bin\//')
      ;;
    Linux*)
      EXPECTED_SUBSTR=$2/$3
      ;;
    *)
      echo "test_libpath not set-up for UNAME=${UNAME}"
      return 77  # skip
      ;;
  esac
  printf "Testing expected libpath output "
  if [ "x${BUILD_MODE}" = xstatic ]; then
    printf "not "
  fi
  printf "containing '${EXPECTED_SUBSTR}' ... "
  if [ "x${USE_OTOOL}" = xyes ]; then
    CMD_OUTPUT=$(otool -l ./$1 | grep -m1 "$2")
  else
    CMD_OUTPUT=$(ldd ./$1 | grep -m1 "$3")
  fi
  case "${CMD_OUTPUT}" in
    *"not found"*)
      echo "failed: ${CMD_OUTPUT}"
      return 1 ;;
    *${EXPECTED_SUBSTR}*) found=yes ;;
    *) found=no ;;
  esac
  if [ "x${BUILD_MODE}" = xstatic ]; then
    if [ "x${found}" = "xyes" ] ; then
      echo "failed: ${CMD_OUTPUT}"
      return 1
    fi
  elif [ "x${found}" = "xno" ] ; then
    echo "failed:"
    if [ "x${USE_OTOOL}" = xyes ]; then
      echo "otool -l ./$1:"
      otool -l ./$1
    else
      echo "ldd ./$1:"
      ldd ./$1
    fi
    return 1
  fi
  echo "passed"
  return 0
}

test_transform(){
  # usage: test_transform ${PROGRAM_TRANSFORM}
  printf "Testing transform ... "
  EXPECTED="easting: 691875.63, northing: 6098907.83, latitude: 55.00, longitude: 12.00"
  case "$1" in
    "${EXPECTED}")
      echo "passed"
      return 0 ;;
    *)
      echo "failed, expected ${EXPECTED}, found $1"
      return 1 ;;
  esac
}

test_searchpath(){
  # usage: test_searchpath ${PROGRAM_SEARCHPATH} ${EXPECTED_SEARCHPATH}
  printf "Testing program searchpath '$1' ... "
  PROGRAM_SEARCHPATH=$1
  UNAME=$(uname)
  case ${UNAME} in
    MINGW* | MSYS*)
      PROGRAM_SEARCHPATH=$(echo "${PROGRAM_SEARCHPATH}" | tr '\\' '/')
  esac
  if [ -z $2 ]; then
    echo "failed (empty expected)"
    return 1
  fi
  case ${PROGRAM_SEARCHPATH} in
    *"$2"*)
      echo "passed"
      return 0 ;;
    *)
      echo "failed, expected '$2'"
      return 1 ;;
  esac
}

test_version(){
  # usage: test_version ${PROGRAM_VERSION} ${EXPECTED_VERSION}
  printf "Testing program version $1 ... "
  if [ -z $2 ]; then
    echo "failed (empty expected)"
    return 1
  fi
  case $1 in
    $2*)
      echo "passed"
      return 0 ;;
    *)
      echo "failed, expected $2"
      return 1 ;;
  esac
}
