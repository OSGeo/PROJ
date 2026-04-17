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

# Replace with just CMAKE_CURRENT_FUNCTION_LIST_DIR once CMake 3.18
set(PROJ_PC_IN_DIR "${CMAKE_CURRENT_LIST_DIR}")

function(configure_proj_pc DIRECTORY)
  set(prefix ${CMAKE_INSTALL_PREFIX})
  set_variable_from_rel_or_absolute_path("libdir" "\${prefix}" "${CMAKE_INSTALL_LIBDIR}")
  set_variable_from_rel_or_absolute_path("includedir" "\${prefix}" "${CMAKE_INSTALL_INCLUDEDIR}")
  set_variable_from_rel_or_absolute_path("datarootdir" "\${prefix}" "${CMAKE_INSTALL_DATAROOTDIR}")
  set(datadir "\${datarootdir}")
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

  # Join lists with a space
  list(JOIN EXTRA_LIBS " " EXTRA_LIBS)
  list(JOIN EXTRA_REQUIRES " " EXTRA_REQUIRES)

  configure_file(
    ${PROJ_PC_IN_DIR}/proj.pc.in
    ${DIRECTORY}/proj.pc
    @ONLY)
endfunction()

# Takes two directories, and create the pkg-config files both
# for the configure time environment and the install environment.
# BUILD_DIRECTORY is where the configure time version should be written
# INSTALL_DIRECTORY is the argument that would otherwise be provided to install(FILES ... DIRECTORY)
function(configure_and_install_proj_pc BUILD_DIRECTORY INSTALL_DIRECTORY)
  configure_proj_pc(${BUILD_DIRECTORY})
  # This is kind of ridiculous, basically we save all the variables
  # that are used to generate the proj.pc except for the INSTALL_PREFIX.
  # Then at install time, we load all those variables, include this script file,
  # and calculate the output directory potentially in relation to the INSTALL_PREFIX.
  # Lastly, the file gets configured again, and written, *not to the install directory itself*
  # but instead to the install directory *inside* the DESTDIR environment variable.
  # See: https://cmake.org/cmake/help/latest/envvar/DESTDIR.html
  install(CODE "\
    cmake_minimum_required(VERSION ${CMAKE_MINIMUM_REQUIRED_VERSION})\n\
    include(\"${PROJ_PC_IN_DIR}/policies.cmake\")\n\
    set(PROJ_VERSION ${PROJ_VERSION})\n\
    set(CMAKE_THREAD_LIBS_INIT ${CMAKE_THREAD_LIBS_INIT})\n\
    set(USE_PKGCONFIG_REQUIRES ${USE_PKGCONFIG_REQUIRES} CACHE BOOL \"\")\n\
    set(TIFF_ENABLED ${TIFF_ENABLED})\n\
    set(CURL_ENABLED ${CURL_ENABLED})\n\
    set(WIN32 ${WIN32})\n\
    set(MINGW ${MINGW})\n\
    set(CMAKE_CXX_IMPLICIT_LINK_LIBRARIES ${CMAKE_CXX_IMPLICIT_LINK_LIBRARIES})\n\
    set(CMAKE_C_IMPLICIT_LINK_LIBRARIES ${CMAKE_C_IMPLICIT_LINK_LIBRARIES})\n\
    set(HAVE_LIBM ${HAVE_LIBM})\n\
    set(HAVE_LIBDL ${HAVE_LIBDL})\n\
    set(CMAKE_INSTALL_LIBDIR \"${CMAKE_INSTALL_LIBDIR}\")\n\
    set(CMAKE_INSTALL_INCLUDEDIR \"${CMAKE_INSTALL_INCLUDEDIR}\")\n\
    set(CMAKE_INSTALL_DATAROOTDIR \"${CMAKE_INSTALL_DATAROOTDIR}\")\n\
    set(INSTALL_DIRECTORY \"${INSTALL_DIRECTORY}\")\n\
    include(\"${PROJ_PC_IN_DIR}/ProjUtilities.cmake\")\n\
    set_variable_from_rel_or_absolute_path(\"OUT_DIRECTORY\" \"\${CMAKE_INSTALL_PREFIX}\" \"\${INSTALL_DIRECTORY}\")\n\
    configure_proj_pc(\"\$ENV{DESTDIR}/\${OUT_DIRECTORY}\")\n\
    message(STATUS \"Generated pkg-config file at \$ENV{DESTDIR}/\${OUT_DIRECTORY}/proj.pc\")\n\
  ")
endfunction()
