# Common CMake macros for post-install tests

include(CTest)

macro(add_test_ldd EXE)
  set(EXEPATH ${CMAKE_BINARY_DIR}/${EXE})
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
      COMMAND sh -c "${LDD_CL} ${EXEPATH} | grep proj")
    if($ENV{BUILD_MODE} STREQUAL "static")
      set_tests_properties(test_ldd PROPERTIES
        PASS_REGULAR_EXPRESSION "^$"
        FAIL_REGULAR_EXPRESSION "${EXPECTED_LDD_CL_OUT};not found"
      )
    else()
      set_tests_properties(test_ldd PROPERTIES
        PASS_REGULAR_EXPRESSION "${EXPECTED_LDD_CL_OUT}"
        FAIL_REGULAR_EXPRESSION "not found"
      )
    endif()
  else()
    add_test(NAME test_ldd COMMAND ${EXEPATH})
    set_tests_properties(test_ldd PROPERTIES SKIP_RETURN_CODE 1)
  endif()
endmacro()

macro(add_test_transform EXE)
  set(EXEPATH ${CMAKE_BINARY_DIR}/${EXE})
  add_test(NAME test_transform COMMAND ${EXEPATH} -t)
  set(EXPECTED
"easting: 691875.63, northing: 6098907.83, latitude: 55.00, longitude: 12.00")
  set_tests_properties(test_transform PROPERTIES
    PASS_REGULAR_EXPRESSION "${EXPECTED}"
  )
endmacro()

macro(add_test_searchpath EXE)
  set(EXEPATH ${CMAKE_BINARY_DIR}/${EXE})
  # data directory property not available, so recreate one
  get_filename_component(EXPECTED_SEARCHPATH
    "${${USE_PROJ_NAME}_DIR}/../../../share/proj" ABSOLUTE)
  if(WIN32)
    # Match each '/' with either '\' or '/'
    string(REPLACE "/" "[\\/]" EXPECTED_SEARCHPATH "${EXPECTED_SEARCHPATH}")
  endif()
  add_test(NAME test_searchpath COMMAND ${EXEPATH} -s)
  set_tests_properties(test_searchpath PROPERTIES
    PASS_REGULAR_EXPRESSION "${EXPECTED_SEARCHPATH}"
  )
endmacro()

macro(add_test_version EXE)
  set(EXEPATH ${CMAKE_BINARY_DIR}/${EXE})
  add_test(NAME test_version COMMAND ${EXEPATH} -v)
  set_tests_properties(test_version PROPERTIES
    PASS_REGULAR_EXPRESSION "${${USE_PROJ_NAME}_VERSION}"
  )
endmacro()
