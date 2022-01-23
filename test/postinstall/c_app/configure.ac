AC_INIT([c_app], [0.1])
AM_INIT_AUTOMAKE
AC_PROG_CC
AC_CONFIG_MACRO_DIR([../m4])

PKG_CHECK_MODULES([PROJ], [proj])

AC_ARG_ENABLE([static-proj],
  AS_HELP_STRING([--enable-static-proj],
    [Enable linking to static PROJ library]))

# If enabled, force static linking for Linux
if test "x$enable_static_proj" = "xyes" -a "x$(uname)" = "xLinux"; then
  PROJ_LIBS=$(echo "$PROJ_LIBS" | sed 's/-lproj/-Wl,-Bstatic -lproj -Wl,-Bdynamic/')
fi

AC_CONFIG_FILES([Makefile])
AC_OUTPUT
