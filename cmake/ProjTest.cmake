#
# Add command-line interface tests
#

function(proj_test_set_properties TESTNAME COST)
  set(_env
    "PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES"
    "PROJ_DATA=${PROJ_BINARY_DIR}/data/for_tests"
  )
  if(NOT TIFF_ENABLED)
    list(APPEND _env "TIFF_ENABLED=NO")
  endif()
  set_tests_properties(${TESTNAME}
    PROPERTIES
      ENVIRONMENT "${_env}"
      COST ${COST}
  )
endfunction()

function(proj_add_test_script_sh SH_NAME EXE_PATH) # [COST (default 1.0)]
  set(testname ${SH_NAME})
  if(${ARGC} GREATER 2)  # Use optional COST argument
    set(_cost ${ARGV2})
  else()
    set(_cost 1.0)
  endif()

  add_test(NAME ${testname}
    WORKING_DIRECTORY ${PROJ_BINARY_DIR}/test/cli
    COMMAND bash ${PROJ_SOURCE_DIR}/test/cli/${SH_NAME} "${${EXE_PATH}}"
  )
  proj_test_set_properties(${testname} ${_cost})
endfunction()

function(proj_run_cli_test TESTFILE EXE_PATH) # [COST (default 1.0)]
  set(testname ${TESTFILE})
  if(${ARGC} GREATER 2)  # Use optional COST argument
    set(_cost ${ARGV2})
  else()
    set(_cost 1.0)
  endif()

  add_test(NAME ${testname}
    WORKING_DIRECTORY ${PROJ_BINARY_DIR}/test/cli
    COMMAND ${Python_EXECUTABLE}
      ${PROJ_SOURCE_DIR}/test/cli/run_cli_test.py
      --exe "${${EXE_PATH}}"
      ${PROJ_SOURCE_DIR}/test/cli/${TESTFILE}
  )
  proj_test_set_properties(${testname} ${_cost})
endfunction()

function(proj_add_gie_test TESTNAME TESTCASE) # [COST (default 1.0)]
  set(GIE_BIN $<TARGET_FILE_NAME:gie>)
  set(TESTFILE ${PROJ_SOURCE_DIR}/test/${TESTCASE})
  if(${ARGC} GREATER 2)  # Use optional COST argument
    set(_cost ${ARGV2})
  else()
    set(_cost 1.0)
  endif()

  add_test(NAME ${TESTNAME}
    WORKING_DIRECTORY ${PROJ_SOURCE_DIR}/test
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GIE_BIN}
    ${TESTFILE}
  )
  proj_test_set_properties(${TESTNAME} ${_cost})
endfunction()

# Create user writable directory for tests
add_custom_target(create_tmp_user_writable_dir ALL
                  COMMAND ${CMAKE_COMMAND} -E remove_directory ${PROJ_BINARY_DIR}/tmp_user_writable_dir
                  COMMAND ${CMAKE_COMMAND} -E make_directory ${PROJ_BINARY_DIR}/tmp_user_writable_dir)

function(proj_add_gie_network_dependent_test TESTNAME TESTCASE) # [COST (default 0.0)]
  set(GIE_BIN $<TARGET_FILE_NAME:gie>)
  set(TESTFILE ${PROJ_SOURCE_DIR}/test/${TESTCASE})
  add_test(NAME ${TESTNAME}
    WORKING_DIRECTORY ${PROJ_SOURCE_DIR}/test
    COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GIE_BIN}
    ${TESTFILE}
  )
  set_property(TEST ${TESTNAME}
    PROPERTY ENVIRONMENT
      "PROJ_USER_WRITABLE_DIRECTORY=${PROJ_BINARY_DIR}/tmp_user_writable_dir"
      "PROJ_NETWORK=ON"
      "PROJ_DATA=${PROJ_BINARY_DIR}/data/for_tests"
  )
  if(${ARGC} GREATER 2)  # Use optional COST argument
    set_property(TEST ${TESTNAME} PROPERTY COST ${ARGV2})
  endif()
endfunction()
