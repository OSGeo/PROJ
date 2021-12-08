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

  if [ ${BUILD_MODE} = shared ]; then
    case $(uname) in
      MINGW* | MSYS*)
        prefix=$(cygpath -u ${prefix})
        export LD_LIBRARY_PATH=${prefix}/bin
        ;;
      Darwin*)
        export DYLD_LIBRARY_PATH=${prefix}/lib
        export LDFLAGS="${LDFLAGS} -Wl,-rpath,${prefix}/lib"
        ;;
      *)
        export LD_LIBRARY_PATH=${prefix}/lib
        ;;
    esac
  fi
}

test_ldd(){
  # usage: test_ldd ${PROGRAM} ${LIBNAME}
  # use optional 'BUILD_MODE=static' to to pass if LIBNAME is not found
  if [ ! $(which ldd) ]; then
    UNAME=$(uname)
    case ${UNAME} in
      Darwin*) alias ldd="otool -L" ;;
      *)
        echo "no ldd equivalent found for UNAME=${UNAME}"
        return 77 ;;  # skip
    esac
  fi
  if [ -n "${LD_LIBRARY_PATH}" ]; then
    EXPECTED_LDD_SUBSTR="${LD_LIBRARY_PATH}/$2"
  elif [ -n "${DYLD_LIBRARY_PATH}" ]; then
    EXPECTED_LDD_SUBSTR="${DYLD_LIBRARY_PATH}/$2"
  else
    EXPECTED_LDD_SUBSTR=$2
  fi
  printf "Testing expected ldd output "
  if [ "x${BUILD_MODE}" = xstatic ]; then
    printf "not "
  fi
  printf "containing '${EXPECTED_LDD_SUBSTR}' ... "
  LDD_OUTPUT=$(ldd ./$1 | grep "$3")
  case "${LDD_OUTPUT}" in
    *"not found"*)
      echo "failed: ${LDD_OUTPUT}"
      return 1 ;;
    *${EXPECTED_LDD_SUBSTR}*) found=yes ;;
    *) found=no ;;
  esac
  if [ "x${BUILD_MODE}" = xstatic ]; then
    if [ "x${found}" = "xyes" ] ; then
      echo "failed: ${LDD_OUTPUT}"
      return 1
    fi
  elif [ "x${found}" = "xno" ] ; then
    echo "failed:"
    ldd ./$1
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
