cmake_minimum_required(VERSION 3.5)
project(testappprojinfo LANGUAGES C)

set(USE_PROJ_NAME "PROJ"
  CACHE STRING "Either PROJ (default) or PROJ4")

find_package(${USE_PROJ_NAME})

# Show some target properties
get_cmake_property(_variableNames VARIABLES)
list(SORT _variableNames)
foreach(_variableName ${_variableNames})
  string(REGEX MATCH "^${USE_PROJ_NAME}_" _matched ${_variableName})
  if(NOT ${_matched} STREQUAL "")
    message(STATUS "${_variableName}=${${_variableName}}")
  endif()
endforeach()

add_executable(testappprojinfo testappprojinfo.c)
target_link_libraries(testappprojinfo PRIVATE ${${USE_PROJ_NAME}_LIBRARIES})

include(CTest)

if(APPLE)
  set(LDD_CL "otool -L")
  set(EXPECTED_LDD_CL_OUT "@rpath/libproj")
elseif(UNIX)
  set(LDD_CL "ldd")
  set(EXPECTED_LDD_CL_OUT "${CMAKE_PREFIX_PATH}/lib/libproj")
elseif(CMAKE_GENERATOR STREQUAL "MSYS Makefiles")
  set(LDD_CL "ldd")
  # Convert to Unix-style path
  execute_process(
    COMMAND cygpath -u ${CMAKE_PREFIX_PATH}/bin/libproj
	OUTPUT_VARIABLE EXPECTED_LDD_CL_OUT
	OUTPUT_STRIP_TRAILING_WHITESPACE)
endif()

if(LDD_CL)
  add_test(NAME test_ldd
    COMMAND sh -c "${LDD_CL} ${CMAKE_BINARY_DIR}/testappprojinfo | grep proj")
  set_tests_properties(test_ldd PROPERTIES
    PASS_REGULAR_EXPRESSION ${EXPECTED_LDD_CL_OUT}
  )
else()
  add_test(NAME test_ldd COMMAND testappprojinfo)
  set_tests_properties(test_ldd PROPERTIES SKIP_RETURN_CODE 1)
endif()

# data directory property not available, so recreate one
get_filename_component(EXPECTED_DATADIR
  "${${USE_PROJ_NAME}_DIR}/../../../share/proj" ABSOLUTE)
if(WIN32)
  # Match each '/' with either '\' or '/'
  string(REPLACE "/" "[\\/]" EXPECTED_DATADIR ${EXPECTED_DATADIR})
endif()
add_test(NAME test_searchpath COMMAND testappprojinfo -s)
set_tests_properties(test_searchpath PROPERTIES
  PASS_REGULAR_EXPRESSION "${EXPECTED_DATADIR}"
)

add_test(NAME test_version COMMAND testappprojinfo -v)
set_tests_properties(test_version PROPERTIES
  PASS_REGULAR_EXPRESSION "${${USE_PROJ_NAME}_VERSION}"
)
