dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([PROJ], [8.0.0],
        [https://github.com/OSGeo/PROJ/issues], proj, [https://proj.org])
AC_CONFIG_MACRO_DIR([m4])
AC_LANG(C)

AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE([subdir-objects])
AM_CONFIG_HEADER(src/proj_config.h)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CC_C99
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL

PKG_PROG_PKG_CONFIG

dnl Enable as much warnings as possible
AX_CFLAGS_WARN_ALL(C_WFLAGS)
AX_CXXFLAGS_WARN_ALL(CXX_WFLAGS)

dnl For ICC: it needs -we10006 instead of -Werror to turn unknown options to errors
dnl Some gcc/clang versions might succeed on this test, so also include -Werror in ERROR_ON_UNKNOWN_OPTIONS
AX_CHECK_COMPILE_FLAG([-Werror -we10006],[ERROR_ON_UNKNOWN_OPTIONS="-Werror -we10006"],[ERROR_ON_UNKNOWN_OPTIONS="-Werror"])

dnl A few ICC warnings to turn off
dnl warning #188: enumerated type mixed with another type  (needed on libcsf)
dnl warning #1684: conversion from pointer to same-sized integral type (potential portability problem)  (needed on frmts/mrf)
dnl warning #2259: non-pointer conversion from "size_t={unsigned long}" to "int" may lose significant bits
dnl warning #2304: non-explicit constructor with single argument may cause implicit type conversion
dnl warning #3280: declaration hides member
dnl remark #11074: Inlining inhibited by limit max-size
dnl remark #11076: To get full report use -qopt-report=4 -qopt-report-phase ipo
AX_CHECK_COMPILE_FLAG([-diag-disable 188,1684,2259,2304,3280,11074,11076],[C_WFLAGS="$C_WFLAGS -diag-disable 188,1684,2259,2304,3280,11074,11076" CXX_WFLAGS="$CXX_WFLAGS -diag-disable 188,1684,2259,2304,3280,11074,11076"],,[$ERROR_ON_UNKNOWN_OPTIONS])

AX_CHECK_COMPILE_FLAG([-Wextra],[C_WFLAGS="$C_WFLAGS -Wextra" CXX_WFLAGS="$CXX_WFLAGS -Wextra"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Winit-self],[C_WFLAGS="$C_WFLAGS -Winit-self" CXX_WFLAGS="$CXX_WFLAGS -Winit-self"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wunused-parameter], [C_WFLAGS="$C_WFLAGS -Wunused-parameter" CXX_WFLAGS="$CXX_WFLAGS -Wunused-parameter" NO_UNUSED_PARAMETER_FLAG="-Wno-unused-parameter"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wmissing-prototypes], [C_WFLAGS="$C_WFLAGS -Wmissing-prototypes"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wmissing-declarations], [C_WFLAGS="$C_WFLAGS -Wmissing-declarations"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wformat], [C_WFLAGS="$C_WFLAGS -Wformat" CXX_WFLAGS="$CXX_WFLAGS -Wformat"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wformat -Werror=format-security -Wno-format-nonliteral], [C_WFLAGS="$C_WFLAGS -Werror=format-security -Wno-format-nonliteral" CXX_WFLAGS="$CXX_WFLAGS -Werror=format-security -Wno-format-nonliteral"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wshorten-64-to-32], [C_WFLAGS="$C_WFLAGS -Wshorten-64-to-32" CXX_WFLAGS="$CXX_WFLAGS -Wshorten-64-to-32"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wlogical-op], [C_WFLAGS="$C_WFLAGS -Wlogical-op" CXX_WFLAGS="$CXX_WFLAGS -Wlogical-op" NO_LOGICAL_OP_FLAG="-Wno-logical-op"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wshadow], [C_WFLAGS="$C_WFLAGS -Wshadow" CXX_WFLAGS="$CXX_WFLAGS -Wshadow"],,[$ERROR_ON_UNKNOWN_OPTIONS])

dnl Error out on things that will fail with MSVC
AX_CHECK_COMPILE_FLAG([-Werror=vla], [C_WFLAGS="$C_WFLAGS -Werror=vla" CXX_WFLAGS="$CXX_WFLAGS -Werror=vla"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Werror=declaration-after-statement], [C_WFLAGS="$C_WFLAGS -Wdeclaration-after-statement"],,[$ERROR_ON_UNKNOWN_OPTIONS])

dnl -Wclobbered is not reliable on most gcc versions
dnl AX_CHECK_COMPILE_FLAG([-Wno-clobbered], [C_WFLAGS="$C_WFLAGS -Wno-clobbered" CXX_WFLAGS="$CXX_WFLAGS -Wno-clobbered"],,[$ERROR_ON_UNKNOWN_OPTIONS])

dnl Warn when macros __TIME__, __DATE__ or __TIMESTAMP__ are encountered as they might prevent bit-wise-identical reproducible compilations. 
AX_CHECK_COMPILE_FLAG([-Wdate-time], [C_WFLAGS="$C_WFLAGS -Wdate-time" CXX_WFLAGS="$CXX_WFLAGS -Wdate-time"],,[$ERROR_ON_UNKNOWN_OPTIONS])

dnl GCC 6 warnings
AX_CHECK_COMPILE_FLAG([-Wnull-dereference], [C_WFLAGS="$C_WFLAGS -Wnull-dereference" CXX_WFLAGS="$CXX_WFLAGS -Wnull-dereference"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wduplicated-cond], [C_WFLAGS="$C_WFLAGS -Wduplicated-cond" CXX_WFLAGS="$CXX_WFLAGS -Wduplicated-cond"],,[$ERROR_ON_UNKNOWN_OPTIONS])

dnl GCC 7 warnings
dnl Do not enable yet. Causes warning in alg/gdalthinplate.cpp due to armadillo templates
dnl AX_CHECK_COMPILE_FLAG([-Wduplicated-branches], [C_WFLAGS="$C_WFLAGS -Wduplicated-branches" CXX_WFLAGS="$CXX_WFLAGS -Wduplicated-branches"],,[$ERROR_ON_UNKNOWN_OPTIONS])

dnl GCC 8 warnings
AC_LANG_PUSH([C++])
AX_CHECK_COMPILE_FLAG([-Wextra-semi], [CXX_WFLAGS="$CXX_WFLAGS -Wextra-semi"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AC_LANG_POP([C++])

AX_CHECK_COMPILE_FLAG([-Wno-sign-compare], [NO_SIGN_COMPARE="-Wno-sign-compare"],,[$ERROR_ON_UNKNOWN_OPTIONS])

dnl clang >= 3.9
AX_CHECK_COMPILE_FLAG([-Wcomma], [C_WFLAGS="$C_WFLAGS -Wcomma" CXX_WFLAGS="$CXX_WFLAGS -Wcomma"],,[$ERROR_ON_UNKNOWN_OPTIONS])

dnl clang and gcc 5
AX_CHECK_COMPILE_FLAG([-Wfloat-conversion], [C_WFLAGS="$C_WFLAGS -Wfloat-conversion" CXX_WFLAGS="$CXX_WFLAGS -Wfloat-conversion"],,[$ERROR_ON_UNKNOWN_OPTIONS])

dnl clang >= 3.2
AX_CHECK_COMPILE_FLAG([-Wdocumentation -Wno-documentation-deprecated-sync], [C_WFLAGS="$C_WFLAGS -Wdocumentation -Wno-documentation-deprecated-sync" CXX_WFLAGS="$CXX_WFLAGS -Wdocumentation -Wno-documentation-deprecated-sync"],,[$ERROR_ON_UNKNOWN_OPTIONS])

dnl C++ specific stuff

AC_LANG_PUSH([C++])
AX_CHECK_COMPILE_FLAG([-Wunused-private-field], [CXX_WFLAGS="$CXX_WFLAGS -Wunused-private-field"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wmissing-declarations], [CXX_WFLAGS="$CXX_WFLAGS -Wmissing-declarations"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wnon-virtual-dtor], [CXX_WFLAGS="$CXX_WFLAGS -Wnon-virtual-dtor" NO_NON_VIRTUAL_DTOR_FLAG="-Wno-non-virtual-dtor"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wold-style-cast], [WARN_OLD_STYLE_CAST="-Wold-style-cast"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Weffc++], [WARN_EFFCPLUSPLUS="-Weffc++"],,[$ERROR_ON_UNKNOWN_OPTIONS])

dnl g++-4.8 complain a bit too much with -Weffc++
if test "$WARN_EFFCPLUSPLUS" != ""; then
    SAVED_CXXFLAGS=$CXXFLAGS
    CXXFLAGS="$CXXFLAGS $WARN_EFFCPLUSPLUS -Werror"
    AC_MSG_CHECKING([if -Weffc++ should be enabled])
    AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
    [[
    class Base {};
    class A: public Base {};
    ]])],
    [AC_MSG_RESULT([yes])],
    [WARN_EFFCPLUSPLUS=""]
    [AC_MSG_RESULT([no])])
    CXXFLAGS="$SAVED_CXXFLAGS"
    CXX_WFLAGS="$CXX_WFLAGS $WARN_EFFCPLUSPLUS"
fi

dnl Clang enables -Woverloaded-virtual if -Wall is defined, but not GCC
AX_CHECK_COMPILE_FLAG([-Woverloaded-virtual], [CXX_WFLAGS="$CXX_WFLAGS -Woverloaded-virtual"],,[$ERROR_ON_UNKNOWN_OPTIONS])

AX_CHECK_COMPILE_FLAG([-Wweak-vtables], [CXX_WFLAGS="$CXX_WFLAGS -Wweak-vtables"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wdeprecated], [CXX_WFLAGS="$CXX_WFLAGS -Wdeprecated"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wabstract-vbase-init], [CXX_WFLAGS="$CXX_WFLAGS -Wabstract-vbase-init"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Winconsistent-missing-destructor-override], [CXX_WFLAGS="$CXX_WFLAGS -Winconsistent-missing-destructor-override"],,[$ERROR_ON_UNKNOWN_OPTIONS])

HAVE_GCC_WARNING_ZERO_AS_NULL_POINTER_CONSTANT=no
AX_CHECK_COMPILE_FLAG([-Wzero-as-null-pointer-constant], [CXX_WFLAGS="$CXX_WFLAGS -Wzero-as-null-pointer-constant" HAVE_GCC_WARNING_ZERO_AS_NULL_POINTER_CONSTANT=yes NO_ZERO_AS_NULL_POINTER_CONSTANT_FLAG="-Wno-zero-as-null-pointer-constant"],,[$ERROR_ON_UNKNOWN_OPTIONS])
if test "$HAVE_GCC_WARNING_ZERO_AS_NULL_POINTER_CONSTANT" = "yes"; then
AC_DEFINE_UNQUOTED(HAVE_GCC_WARNING_ZERO_AS_NULL_POINTER_CONSTANT, 1,
          [Define to 1 if the compiler supports -Wzero-as-null-pointer-constant])
fi
AC_LANG_POP([C++])

dnl ---------------------------------------------------------------------------
dnl Check for --enable-lto
dnl ---------------------------------------------------------------------------

AC_MSG_CHECKING([to enable LTO (link time optimization) build])

AC_ARG_ENABLE(lto,
              AS_HELP_STRING([--enable-lto],
                             [enable LTO(link time optimization) (disabled by default)]))

FLTO_FLAG=""
if test "x$enable_lto" = "xyes"; then
    AC_LANG_PUSH([C++])
    AX_CHECK_COMPILE_FLAG([-flto], [FLTO_FLAG="-flto"],,[$ERROR_ON_UNKNOWN_OPTIONS])
    if test "$FLTO_FLAG" != ""; then
        SAVED_CXXFLAGS=$CXXFLAGS
        CXXFLAGS="$CXXFLAGS $FLTO_FLAG -Werror"
        AC_MSG_CHECKING([if -flto is available at link time])
        AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [[]])],
        [AC_MSG_RESULT([yes])],
        [FLTO_FLAG=""]
        [AC_MSG_RESULT([no])])
        CXXFLAGS="$SAVED_CXXFLAGS"
    fi
    AC_LANG_POP([C++])
    AX_CHECK_COMPILE_FLAG([-Wodr], [CXX_WFLAGS="$CXX_WFLAGS -Wodr"],,[$ERROR_ON_UNKNOWN_OPTIONS])
else
    AC_MSG_RESULT([no])
fi
AC_SUBST(FLTO_FLAG,$FLTO_FLAG)

dnl Result in better inlining, but larger file
dnl AX_CHECK_COMPILE_FLAG([-fno-semantic-interposition], [CFLAGS="$CFLAGS -fno-semantic-interposition" CXXFLAGS="$CXXFLAGS -fno-semantic-interposition"],,[$ERROR_ON_UNKNOWN_OPTIONS])

AC_SUBST(C_WFLAGS,$C_WFLAGS)
AC_SUBST(CXX_WFLAGS,$CXX_WFLAGS)
AC_SUBST(NO_ZERO_AS_NULL_POINTER_CONSTANT_FLAG,$NO_ZERO_AS_NULL_POINTER_CONSTANT_FLAG)

CFLAGS="${CFLAGS} -fvisibility=hidden"
CXXFLAGS="${CXXFLAGS} -fvisibility=hidden"

case "${host_os}" in
    cygwin* | mingw32* | pw32* | beos* | darwin*)
        CFLAGS="${CFLAGS} -DNOMINMAX"
        CXXFLAGS="${CXXFLAGS} -DNOMINMAX"
        ;;
    *)
        ;;
esac

dnl Checks for libraries.
save_CFLAGS="$CFLAGS"
CFLAGS=`echo "$CFLAGS" | sed "s/-Werror/ /"`
AC_CHECK_LIB(m,exp,,,)
CFLAGS="$save_CFLAGS"

dnl We check for headers
AC_HEADER_STDC

dnl Check flag for accurate arithmetic with Intel compiler.  This is
dnl needed to stop the compiler from ignoring parentheses in expressions
dnl like (a + b) + c and from simplifying 0.0 + x to x (which is wrong if
dnl x = -0.0).
AX_CHECK_COMPILE_FLAG([-fp-model precise],
        [CFLAGS="$CFLAGS -fp-model precise"],,[-Werror])

AC_SEARCH_LIBS([sqrt], [m])

AC_CHECK_FUNC(localeconv, [AC_DEFINE(HAVE_LOCALECONV,1,[Define to 1 if you have localeconv])])
AC_CHECK_FUNCS([strerror])
AC_CHECK_LIB(dl,dladdr,,,)

dnl ---------------------------------------------------------------------------
dnl Provide a mechanism to disable real mutex support (if lacking win32 or
dnl posix mutexes for instance).
dnl ---------------------------------------------------------------------------

AC_ARG_WITH([mutex],
            AS_HELP_STRING([--without-mutex],
               [Disable real mutex locks (lacking pthreads)]),,)

AC_MSG_CHECKING([for mutexes])
THREAD_LIB=""
if test "$with_mutex" = yes -o x"$with_mutex" = x ; then
    MUTEX_SETTING=pthread
    AC_CHECK_LIB(pthread,pthread_create,PTHREAD_EXISTS=YES,,,)
    if test -n "$PTHREAD_EXISTS" ; then
        THREAD_LIB="-lpthread"
    fi
    AC_CHECK_LIB(pthread,pthread_mutexattr_settype,,,)
    AC_CHECK_DECL(PTHREAD_MUTEX_RECURSIVE,
                  AC_DEFINE(HAVE_PTHREAD_MUTEX_RECURSIVE, [], [Define if your pthreads implementation have PTHREAD_MUTEX_RECURSIVE]),
                  ,
                  [#include <pthread.h>])
    AC_MSG_RESULT([enabled, pthread])
else
    MUTEX_SETTING=stub
    AC_MSG_RESULT([disabled by user])
fi

AC_SUBST(MUTEX_SETTING,$MUTEX_SETTING)
AC_SUBST(THREAD_LIB,$THREAD_LIB)

dnl ---------------------------------------------------------------------------
dnl Check for sqlite3 library and binary
dnl ---------------------------------------------------------------------------

if test "x$SQLITE3_CFLAGS$SQLITE3_LIBS" = "x" ; then

    dnl Would build and run with older versions, but with horrible performance
    dnl See https://github.com/OSGeo/PROJ/issues/1718

    PKG_CHECK_MODULES([SQLITE3], [sqlite3 >= 3.11])
fi
AC_SUBST(SQLITE3_CFLAGS,$SQLITE3_CFLAGS)
AC_SUBST(SQLITE3_LIBS,$SQLITE3_LIBS)

AC_CHECK_PROG(SQLITE3_CHECK,sqlite3,yes)
if test x"$SQLITE3_CHECK" != x"yes" ; then
    AC_MSG_ERROR([Please install sqlite3 binary.])
fi

dnl ---------------------------------------------------------------------------
dnl Check for libtiff
dnl ---------------------------------------------------------------------------

AC_ARG_ENABLE([tiff],
              AS_HELP_STRING([--enable-tiff],
                    [Enable TIFF support; strongly encouraged to read some grids [default=yes]]))

if test "x$enable_tiff" != "xno" -o "x$enable_tiff" = ""; then
    if test "x$TIFF_CFLAGS$TIFF_LIBS" = "x" ; then
        if $PKG_CONFIG libtiff; then
            PKG_CHECK_MODULES([TIFF], [libtiff])
        else
            PKG_CHECK_MODULES([TIFF], [libtiff-4])
        fi
    fi
    TIFF_ENABLED_FLAGS=-DTIFF_ENABLED
fi
AC_SUBST(TIFF_CFLAGS,$TIFF_CFLAGS)
AC_SUBST(TIFF_LIBS,$TIFF_LIBS)
AC_SUBST(TIFF_ENABLED_FLAGS,$TIFF_ENABLED_FLAGS)

dnl ---------------------------------------------------------------------------
dnl Check for curl
dnl ---------------------------------------------------------------------------

FOUND_CURL=no
CURL_CFLAGS=
CURL_LIB=

AC_ARG_WITH(curl,
    [  --with-curl[=ARG]         Enable curl support (ARG=path to curl-config.)],,,)

dnl Clear some cache variables
unset ac_cv_path_LIBCURL

if test "`basename xx/$with_curl`" = "curl-config" ; then
  LIBCURL_CONFIG="$with_curl"
elif test "$with_curl" = "no" ; then
  LIBCURL_CONFIG=no
else
  AC_PATH_PROG(LIBCURL_CONFIG, curl-config, not-found)
fi

if test "$LIBCURL_CONFIG" = "not-found" ; then
  AC_MSG_ERROR([curl not found. If wanting to do a build without curl support (and thus without built-in networking capability), explictly disable it with --without-curl])
elif test "$LIBCURL_CONFIG" != "no" ; then

  CURL_VERNUM=`$LIBCURL_CONFIG --vernum`
  CURL_VER=`$LIBCURL_CONFIG --version | awk '{print $2}'`

  AC_MSG_RESULT([        found libcurl version $CURL_VER])

  AC_CHECK_LIB(curl,curl_global_init,FOUND_CURL=yes,FOUND_CURL=no,`$LIBCURL_CONFIG --libs`)

  if test "$FOUND_CURL" = "no" ; then
    AC_MSG_ERROR([curl not found. If wanting to do a build without curl support (and thus without built-in networking capability), explictly disable it with --without-curl])
  fi
  CURL_ENABLED_FLAGS=-DCURL_ENABLED
fi

if test "$FOUND_CURL" = "yes" ; then
  CURL_CFLAGS=`$LIBCURL_CONFIG --cflags`
  CURL_LIBS=`$LIBCURL_CONFIG --libs`
fi

AC_SUBST(CURL_CFLAGS,$CURL_CFLAGS)
AC_SUBST(CURL_LIBS,$CURL_LIBS)
AC_SUBST(CURL_ENABLED_FLAGS,$CURL_ENABLED_FLAGS)
AM_CONDITIONAL(HAVE_CURL, [test "x$FOUND_CURL" = "xyes"])

dnl ---------------------------------------------------------------------------
dnl Check for external Google Test
dnl ---------------------------------------------------------------------------

AC_ARG_WITH(external-gtest,
            AS_HELP_STRING([--with-external-gtest],
               [Whether to use external Google Test]),,)

if test "x$with_external_gtest" = "xyes" ; then
    AC_MSG_RESULT([using external GTest.])
    PKG_CHECK_MODULES([GTEST], [gtest >= 1.8.0])
else
    AC_MSG_RESULT([using internal GTest.])
    GTEST_CFLAGS="-I\$(top_srcdir)/test/googletest/include"
    GTEST_LIBS="\$(top_builddir)/test/googletest/libgtest.la"
fi
AM_CONDITIONAL(USE_EXTERNAL_GTEST, [test "x$with_external_gtest" = "xyes"])
AC_SUBST(GTEST_CFLAGS,$GTEST_CFLAGS)
AC_SUBST(GTEST_LIBS,$GTEST_LIBS)

dnl ---------------------------------------------------------------------------
dnl Generate files
dnl ---------------------------------------------------------------------------

AC_CONFIG_FILES([Makefile cmake/Makefile src/Makefile include/Makefile include/proj/Makefile include/proj/internal/Makefile
                 include/proj/internal/nlohmann/Makefile
                 test/Makefile test/cli/Makefile test/gie/Makefile test/gigs/Makefile test/unit/Makefile
                 man/Makefile man/man1/Makefile data/Makefile])
if ! test "x$with_external_gtest" = "xyes" ; then
    AC_CONFIG_FILES([test/googletest/Makefile test/googletest/include/Makefile
                     test/googletest/include/gtest/Makefile
                     test/googletest/include/gtest/internal/Makefile
                     test/googletest/include/gtest/internal/custom/Makefile
                     test/googletest/src/Makefile])
fi

AC_CONFIG_FILES([proj.pc])

AC_OUTPUT
