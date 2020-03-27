#
# add test with sh script
#

function(proj_test_set_properties TESTNAME)
  set_property(TEST ${TESTNAME}
    PROPERTY ENVIRONMENT
      "PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES"
      "PROJ_LIB=${PROJ_BINARY_DIR}/data/for_tests")
endfunction()

function(proj_add_test_script_sh SH_NAME BIN_USE)
  if(UNIX)
    get_filename_component(testname ${SH_NAME} NAME_WE)

    add_test(NAME "${testname}"
      WORKING_DIRECTORY ${PROJ_SOURCE_DIR}/data
      COMMAND bash ${PROJ_SOURCE_DIR}/test/cli/${SH_NAME}
      ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${${BIN_USE}}
    )
    proj_test_set_properties(${testname})

  endif()
endfunction()


function(proj_add_gie_test TESTNAME TESTCASE)

    set(GIE_BIN $<TARGET_FILE_NAME:gie>)
    set(TESTFILE ${PROJ_SOURCE_DIR}/test/${TESTCASE})
    add_test(NAME ${TESTNAME}
      WORKING_DIRECTORY ${PROJ_SOURCE_DIR}/test
      COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GIE_BIN}
      ${TESTFILE}
    )
    proj_test_set_properties(${TESTNAME})

endfunction()
