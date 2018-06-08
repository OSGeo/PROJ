dnl Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT([PROJ], [5.2.0],
        [https://github.com/OSGeo/proj.4/issues], proj, [https://proj4.org])
AC_CONFIG_MACRO_DIR([m4])
AC_LANG(C)

AC_CONFIG_AUX_DIR([.])
AM_INIT_AUTOMAKE
AM_CONFIG_HEADER(src/proj_config.h)

dnl Checks for programs.
AC_PROG_CC
AC_PROG_CXX
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])
AC_PROG_INSTALL
AC_PROG_LN_S
AC_PROG_MAKE_SET
AM_PROG_LIBTOOL

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

dnl gnu89 is a reasonable target to get MSVC compatibility.
dnl but only apply it with gcc, since clang will throw a lot of warnings
SAVED_CFLAGS=$CFLAGS
CFLAGS="$CFLAGS $ERROR_ON_UNKNOWN_OPTIONS -std=gnu89"
AC_MSG_CHECKING([if -std=gnu89 can be enabled])
AC_COMPILE_IFELSE([AC_LANG_PROGRAM(
  [[#if defined(__clang__) || !defined(__GNUC__)
    #error "not gcc"
    #endif]])],
  [C_WFLAGS="$C_WFLAGS -std=gnu89"]
  [AC_MSG_RESULT([yes])],
  [AC_MSG_RESULT([no])])
CFLAGS=$SAVED_CFLAGS

dnl C++ specific stuff

AC_LANG_PUSH([C++])
AX_CHECK_COMPILE_FLAG([-Wunused-private-field], [CXX_WFLAGS="$CXX_WFLAGS -Wunused-private-field"],,[$ERROR_ON_UNKNOWN_OPTIONS])
AX_CHECK_COMPILE_FLAG([-Wmissing-prototypes], [CXX_WFLAGS="$CXX_WFLAGS -Wmissing-prototypes"],,[$ERROR_ON_UNKNOWN_OPTIONS])
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

dnl Forbid use of 'or', 'and', ... alias operators
AX_CHECK_COMPILE_FLAG([-fno-operator-names], [CXX_WFLAGS="$CXX_WFLAGS -fno-operator-names"],,[$ERROR_ON_UNKNOWN_OPTIONS])

HAVE_GCC_WARNING_ZERO_AS_NULL_POINTER_CONSTANT=no
AX_CHECK_COMPILE_FLAG([-Wzero-as-null-pointer-constant], [CXX_WFLAGS="$CXX_WFLAGS -Wzero-as-null-pointer-constant" HAVE_GCC_WARNING_ZERO_AS_NULL_POINTER_CONSTANT=yes NO_ZERO_AS_NULL_POINTER_CONSTANT_FLAG="-Wno-zero-as-null-pointer-constant"],,[$ERROR_ON_UNKNOWN_OPTIONS])
if test "$HAVE_GCC_WARNING_ZERO_AS_NULL_POINTER_CONSTANT" = "yes"; then
AC_DEFINE_UNQUOTED(HAVE_GCC_WARNING_ZERO_AS_NULL_POINTER_CONSTANT, 1,
          [Define to 1 if the compiler supports -Wzero-as-null-pointer-constant])
fi

AC_LANG_POP([C++])

AC_SUBST(C_WFLAGS,$C_WFLAGS)
AC_SUBST(CXX_WFLAGS,$CXX_WFLAGS)
AC_SUBST(NO_ZERO_AS_NULL_POINTER_CONSTANT_FLAG,$NO_ZERO_AS_NULL_POINTER_CONSTANT_FLAG)


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
dnl Check for C99 math functions
save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS -Wall -Werror"
AC_MSG_CHECKING([for C99 math functions])
AC_LINK_IFELSE([AC_LANG_PROGRAM(
        [#include <math.h>],
        [int q;
         return (int)(hypot(3.0, 4.0) + atanh(0.8) + cbrt(8.0) +
                      remquo(100.0, 90.0, &q) +
                      remainder(100.0, 90.0) + copysign(1.0, -0.0) +
                      log1p(0.1) + asinh(0.1)) +
		      isnan(0.0);])],
        [AC_MSG_RESULT([yes]);C99_MATH="-DHAVE_C99_MATH=1"],
        [AC_MSG_RESULT([no]);C99_MATH="-DHAVE_C99_MATH=0"])
CFLAGS="$save_CFLAGS $C99_MATH"

AC_SEARCH_LIBS([sqrt], [m])

AC_CHECK_FUNC(localeconv, [AC_DEFINE(HAVE_LOCALECONV,1,[Define to 1 if you have localeconv])])

dnl ---------------------------------------------------------------------------
dnl Check for JNI support.
dnl ---------------------------------------------------------------------------

JNI_INCLUDE=
export JNI_INCLUDE

AC_ARG_WITH(jni,
            [AS_HELP_STRING([--with-jni=dir],
                            [Include Java/JNI support, add optional include dir])
            ],,)

if test "$with_jni" = "yes" ; then

  AC_DEFINE(JNI_ENABLED,1,[Enabled for Java/JNI Support])

  AC_MSG_CHECKING(whether to enable Java/JNI support)

  AC_MSG_RESULT([enabled])

elif test "$with_jni" != "no" -a "$with_jni" != "" ; then

  AC_MSG_CHECKING(whether to enable Java/JNI support)

  if test \! -r "$with_jni/jni.h" ; then
    AC_MSG_ERROR(Did not find $with_jni/jni.h)
  fi

  AC_DEFINE(JNI_ENABLED,1,[Enabled for Java/JNI Support])

  AC_MSG_RESULT([enabled])

  JNI_INCLUDE="-I$with_jni"

elif test "$with_jni" = "" ; then
  AC_CHECK_HEADERS(jni.h)

  AC_MSG_CHECKING(whether to enable Java/JNI support)

  if test "$ac_cv_header_jni_h" = "no" ; then
    AC_MSG_RESULT([disabled])
  else
    AC_MSG_RESULT([enabled])
    AC_DEFINE(JNI_ENABLED,1,[Enabled for Java/JNI Support])
  fi

else
  AC_MSG_CHECKING(whether to enable Java/JNI support)

  AC_MSG_RESULT([disabled])
fi

AC_SUBST(JNI_INCLUDE,$JNI_INCLUDE)

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

AC_CONFIG_FILES([Makefile cmake/Makefile src/Makefile
                 test/Makefile test/gie/Makefile test/gigs/Makefile test/unit/Makefile
                 test/googletest/Makefile test/googletest/include/Makefile
                 test/googletest/include/gtest/Makefile
                 test/googletest/include/gtest/internal/Makefile
                 test/googletest/include/gtest/internal/custom/Makefile
                 test/googletest/src/Makefile
                 man/Makefile man/man1/Makefile man/man3/Makefile nad/Makefile
                 jniwrap/Makefile jniwrap/org.osgeo.proj/Makefile jniwrap/org.osgeo.proj/org/Makefile jniwrap/org.osgeo.proj/org/proj4/Makefile])
AC_CONFIG_FILES([nad/install], [chmod +x nad/install])

AC_CONFIG_FILES([proj.pc])

AC_OUTPUT
