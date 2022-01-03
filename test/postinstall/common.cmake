# Common CMake macros for post-install tests

include(CTest)

macro(add_test_libpath EXE)
  set(EXEPATH ${CMAKE_BINARY_DIR}/${EXE})
  set(HAS_TOOL TRUE)
  if(APPLE)
    set(EXPECTED_SUBSTR "${CMAKE_PREFIX_PATH}/lib")
  elseif(UNIX)
    set(EXPECTED_SUBSTR "${CMAKE_PREFIX_PATH}/lib/libproj")
  elseif(CMAKE_GENERATOR STREQUAL "MSYS Makefiles")
    # Convert to Unix-style path
    execute_process(
      COMMAND cygpath -u "${CMAKE_PREFIX_PATH}/bin/libproj"
        OUTPUT_VARIABLE EXPECTED_SUBSTR
        OUTPUT_STRIP_TRAILING_WHITESPACE)
  else()
    set(HAS_TOOL FALSE)
  endif()
  if(HAS_TOOL)
    if(APPLE)
      add_test(NAME test_libpath
        COMMAND sh -c "otool -l ${EXEPATH} | grep -m1 \"${EXPECTED_SUBSTR}\"")
    else()
      add_test(NAME test_libpath
        COMMAND sh -c "ldd ${EXEPATH} | grep -m1 proj")
    endif()
    if($ENV{BUILD_MODE} STREQUAL "static")
      set_tests_properties(test_libpath PROPERTIES
        PASS_REGULAR_EXPRESSION "^$"
        FAIL_REGULAR_EXPRESSION "${EXPECTED_SUBSTR};not found"
      )
    else()
      set_tests_properties(test_libpath PROPERTIES
        PASS_REGULAR_EXPRESSION "${EXPECTED_SUBSTR}"
        FAIL_REGULAR_EXPRESSION "not found"
      )
    endif()
  else()
    add_test(NAME test_libpath COMMAND ${EXEPATH})
    set_tests_properties(test_libpath PROPERTIES SKIP_RETURN_CODE 1)
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
