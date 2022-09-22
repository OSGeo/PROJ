################################################################################
# ProjUtilities.cmake - part of CMake configuration of PROJ library
#
# Based on BoostUtilities.cmake from CMake configuration for Boost
################################################################################
# Copyright (C) 2007 Douglas Gregor <doug.gregor@gmail.com>
# Copyright (C) 2007 Troy Straszheim
# Copyright (C) 2010 Mateusz Loskot <mateusz@loskot.net>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   https://www.boost.org/LICENSE_1_0.txt
################################################################################
# Macros in this module:
#
#   print_variable
#   proj_target_output_name
#   configure_proj_pc
#
################################################################################

#
#  pretty-prints the value of a variable so that the
#  equals signs align
#

function(print_variable NAME)
  string(LENGTH "${NAME}" varlen)
  math(EXPR padding_len 30-${varlen})
  if(${padding_len} GREATER 0)
    string(SUBSTRING "                            "
      0 ${padding_len} varpadding)
  endif()
  message(STATUS "${NAME}${varpadding} = ${${NAME}}")
endfunction()

#
# Generates output name for given target depending on platform and version.
# For instance, on Windows, dynamic link libraries get ABI version suffix
# proj_X_Y.dll.
#

function(proj_target_output_name TARGET_NAME OUTPUT_NAME)
  if(NOT DEFINED TARGET_NAME)
    message(SEND_ERROR "Error, the variable TARGET_NAME is not defined!")
  endif()

  if(NOT DEFINED ${PROJECT_NAME}_VERSION)
    message(SEND_ERROR
      "Error, the variable ${${PROJECT_NAME}_VERSION} is not defined!")
  endif()

  # On Windows, ABI version is specified using binary file name suffix.
  # On Unix, suffix is empty and SOVERSION is used instead.
  if(WIN32)
    string(LENGTH "${${PROJECT_NAME}_ABI_VERSION}" abilen)
    if(abilen GREATER 0)
      set(SUFFIX "_${${PROJECT_NAME}_ABI_VERSION}")
    endif()
  endif()

  set(${OUTPUT_NAME} ${TARGET_NAME}${SUFFIX} PARENT_SCOPE)
endfunction()

#
# Configure a pkg-config file proj.pc
# See also ProjInstallPath.cmake
#

function(set_variable_from_rel_or_absolute_path var root rel_or_abs_path)
  if(IS_ABSOLUTE "${rel_or_abs_path}")
    set(${var} "${rel_or_abs_path}" PARENT_SCOPE)
  else()
    set(${var} "${root}/${rel_or_abs_path}" PARENT_SCOPE)
  endif()
endfunction()

function(configure_proj_pc)
  set(prefix ${CMAKE_INSTALL_PREFIX})
  set_variable_from_rel_or_absolute_path("libdir" "$\{prefix\}" "${CMAKE_INSTALL_LIBDIR}")
  set_variable_from_rel_or_absolute_path("includedir" "$\{prefix\}" "${CMAKE_INSTALL_INCLUDEDIR}")
  set_variable_from_rel_or_absolute_path("datarootdir" "$\{prefix\}" "${CMAKE_INSTALL_DATAROOTDIR}")
  set(datadir "$\{datarootdir\}")
  set(PACKAGE "proj")
  set(VERSION ${PROJ_VERSION})
  # Build strings of dependencies (Libs.private, Requires.private)
  set(EXTRA_LIBS "${CMAKE_THREAD_LIBS_INIT}")
  set(EXTRA_REQUIRES "")
  option(USE_PKGCONFIG_REQUIRES "Use 'Requires' instead 'Libs' in proj.pc" ON)
  macro(add_module_or_libs MODULE)
    if(USE_PKGCONFIG_REQUIRES)
      list(APPEND EXTRA_REQUIRES "${MODULE}")
    else()
      list(APPEND EXTRA_LIBS "${ARGN}")
    endif()
  endmacro()
  add_module_or_libs(sqlite3 -lsqlite3)
  if(TIFF_ENABLED)
    add_module_or_libs(libtiff-4 -ltiff)
  endif()
  if(CURL_ENABLED)
    add_module_or_libs(libcurl -lcurl)
  endif()
  if(WIN32 AND NOT MINGW)
    list(APPEND EXTRA_LIBS -lole32 -lshell32)
  else()
    set(cxx_libs "${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES}")
    list(REMOVE_ITEM cxx_libs ${CMAKE_C_IMPLICIT_LINK_LIBRARIES} nonempty)
    foreach(lib IN LISTS cxx_libs)
      list(APPEND EXTRA_LIBS "-l${lib}")
    endforeach()
  endif()
  if(HAVE_LIBM AND NOT "-lm" IN_LIST EXTRA_LIBS)
    list(APPEND EXTRA_LIBS -lm)
  endif()
  if(HAVE_LIBDL)
    list(APPEND EXTRA_LIBS -ldl)
  endif()
  # Join list with a space; list(JOIN) added CMake 3.12
  string(REPLACE ";" " " EXTRA_LIBS "${EXTRA_LIBS}")
  string(REPLACE ";" " " EXTRA_REQUIRES "${EXTRA_REQUIRES}")

  configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/proj.pc.in
    ${CMAKE_CURRENT_BINARY_DIR}/proj.pc
    @ONLY)
endfunction()
