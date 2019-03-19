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
#   proj_target_output_name:
#
################################################################################

#
# A big shout out to the cmake gurus @ compiz
#

function(colormsg)
  string(ASCII 27 _escape)
  set(WHITE "29")
  set(GRAY "30")
  set(RED "31")
  set(GREEN "32")
  set(YELLOW "33")
  set(BLUE "34")
  set(MAG "35")
  set(CYAN "36")

  foreach(color WHITE GRAY RED GREEN YELLOW BLUE MAG CYAN)
    set(HI${color} "1\;${${color}}")
    set(LO${color} "2\;${${color}}")
    set(_${color}_ "4\;${${color}}")
    set(_HI${color}_ "1\;4\;${${color}}")
    set(_LO${color}_ "2\;4\;${${color}}")
  endforeach()

  set(str "")
  set(coloron FALSE)
  foreach(arg ${ARGV})
    if(NOT ${${arg}} STREQUAL "")
      if(CMAKE_COLOR_MAKEFILE)
        set(str "${str}${_escape}[${${arg}}m")
        set(coloron TRUE)
      endif()
    else()
      set(str "${str}${arg}")
      if(coloron)
        set(str "${str}${_escape}[0m")
        set(coloron FALSE)
      endif()
      set(str "${str} ")
    endif()
  endforeach()
  message(STATUS ${str})
endfunction()

# colormsg("Colors:"
#   WHITE "white" GRAY "gray" GREEN "green"
#   RED "red" YELLOW "yellow" BLUE "blue" MAG "mag" CYAN "cyan"
#   _WHITE_ "white" _GRAY_ "gray" _GREEN_ "green"
#   _RED_ "red" _YELLOW_ "yellow" _BLUE_ "blue" _MAG_ "mag" _CYAN_ "cyan"
#   _HIWHITE_ "white" _HIGRAY_ "gray" _HIGREEN_ "green"
#   _HIRED_ "red" _HIYELLOW_ "yellow" _HIBLUE_ "blue" _HIMAG_ "mag"
#   _HICYAN_ "cyan" HIWHITE "white" HIGRAY "gray" HIGREEN "green"
#   HIRED "red" HIYELLOW "yellow" HIBLUE "blue" HIMAG "mag" HICYAN "cyan"
#   "right?")

#
#  pretty-prints the value of a variable so that the
#  equals signs align
#

function(boost_report_value NAME)
  string(LENGTH "${NAME}" varlen)
  # LOG
  #message(STATUS "boost_report_value: NAME=${NAME} (${varlen})")
  #message(STATUS "boost_report_value: \${NAME}=${${NAME}}")
  math(EXPR padding_len 40-${varlen})
  string(SUBSTRING "                                      "
    0 ${padding_len} varpadding)
  colormsg("${NAME}${varpadding} = ${${NAME}}")
endfunction()

#
# Generates output name for given target depending on platform and version.
# For instance, on Windows, libraries get ABI version suffix proj_X_Y.{dll|lib}.
#

function(proj_target_output_name TARGET_NAME OUTPUT_NAME)
  if(NOT DEFINED TARGET_NAME)
    message(SEND_ERROR "Error, the variable TARGET_NAME is not defined!")
  endif()

  if(NOT DEFINED ${PROJECT_INTERN_NAME}_VERSION)
    message(SEND_ERROR
      "Error, the variable ${${PROJECT_INTERN_NAME}_VERSION} is not defined!")
  endif()

  # On Windows, ABI version is specified using binary file name suffix.
  # On Unix, suffix is empty and SOVERSION is used instead.
  if(WIN32)
    string(LENGTH "${${PROJECT_INTERN_NAME}_ABI_VERSION}" abilen)
    if(abilen GREATER 0)
      set(SUFFIX "_${${PROJECT_INTERN_NAME}_ABI_VERSION}")
    endif()
  endif()

  set(${OUTPUT_NAME} ${TARGET_NAME}${SUFFIX} PARENT_SCOPE)
endfunction()
