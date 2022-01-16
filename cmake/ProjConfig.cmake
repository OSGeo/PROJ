################################################################################
# ProjConfig.cmake - CMake build configuration of PROJ library
################################################################################
# Copyright (C) 2010 Mateusz Loskot <mateusz@loskot.net>
#
# Distributed under the Boost Software License, Version 1.0.
# (See accompanying file LICENSE_1_0.txt or copy at
# https://www.boost.org/LICENSE_1_0.txt)
################################################################################
include(CheckLibraryExists)
include(CheckFunctionExists)

# if C flags have -Werror, temporarily remove these while running some checks
string(FIND "${CMAKE_C_FLAGS}" "-Werror" FIND_WERROR)
if(FIND_WERROR GREATER_EQUAL 0)
  string(REPLACE "-Werror" "" _tmp_CMAKE_C_FLAGS ${CMAKE_C_FLAGS})
  set(_prev_CMAKE_C_FLAGS "${CMAKE_C_FLAGS}")
  set(CMAKE_C_FLAGS "${_tmp_CMAKE_C_FLAGS}")
endif()

# check needed include file
check_function_exists(localeconv HAVE_LOCALECONV)
check_function_exists(strerror HAVE_STRERROR)
if(NOT WIN32)
  check_library_exists(dl dladdr "" HAVE_LIBDL)
  check_library_exists(m exp "" HAVE_LIBM)
endif()

# restore CMAKE_C_FLAGS as before
if(FIND_WERROR GREATER_EQUAL 0)
  set(CMAKE_C_FLAGS "${_prev_CMAKE_C_FLAGS}")
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
set(AUTOCONF_PROJ_CONFIG_H "${PROJ_SOURCE_DIR}/src/proj_config.h")
if(EXISTS ${AUTOCONF_PROJ_CONFIG_H})
  message(WARNING
    "Autoconf's ${AUTOCONF_PROJ_CONFIG_H} may interfere with this "
    "CMake build. Run 'make distclean' in the source directory "
    "before CMake's build.")
endif()

configure_file(cmake/proj_config.cmake.in src/proj_config.h)
