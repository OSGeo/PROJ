################################################################################
# ProjConfig.cmake - CMake build configuration of PROJ library
################################################################################
# Copyright (C) 2010 Mateusz Loskot <mateusz@loskot.net>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt)
################################################################################
include(CheckIncludeFiles)
include(CheckLibraryExists)
include(CheckFunctionExists)

# check needed include file
check_include_files(dlfcn.h HAVE_DLFCN_H)
check_include_files(inttypes.h HAVE_INTTYPES_H)
check_include_files(memory.h HAVE_MEMORY_H)
check_include_files(stdint.h HAVE_STDINT_H)
check_include_files(stdlib.h HAVE_STDLIB_H)
check_include_files(string.h HAVE_STRING_H)
check_include_files(sys/stat.h HAVE_SYS_STAT_H)
check_include_files(sys/types.h HAVE_SYS_TYPES_H)
check_include_files(unistd.h HAVE_UNISTD_H)
check_include_files("stdlib.h;stdarg.h;string.h;float.h" STDC_HEADERS)

check_function_exists(localeconv HAVE_LOCALECONV)
check_function_exists(strerror HAVE_STRERROR)
if(NOT WIN32)
    check_library_exists(dl dladdr "" HAVE_LIBDL)
    # check libm need on unix
    check_library_exists(m ceil "" HAVE_LIBM)
endif()

set(PACKAGE "proj")
set(PACKAGE_BUGREPORT "https://github.com/OSGeo/PROJ/issues")
set(PACKAGE_NAME "PROJ")
set(PACKAGE_STRING "PROJ ${${PROJECT_NAME}_VERSION}")
set(PACKAGE_TARNAME "proj")
set(PACKAGE_URL "https://proj.org")
set(PACKAGE_VERSION "${${PROJECT_NAME}_VERSION}")

# check if a second proj_config.h exists (created by ./configure)
# as this is within CMake's C_INCLUDES / CXX_INCLUDES
set(AUTOCONF_PROJ_CONFIG_H "${CMAKE_SOURCE_DIR}/src/proj_config.h")
if(EXISTS ${AUTOCONF_PROJ_CONFIG_H})
  message(WARNING
    "Autoconf's ${AUTOCONF_PROJ_CONFIG_H} may interfere with this "
    "CMake build. Run 'make distclean' in the source directory "
    "before CMake's build.")
endif()

configure_file(cmake/proj_config.cmake.in src/proj_config.h)
