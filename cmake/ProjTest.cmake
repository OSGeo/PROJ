#
# add test with sh script
#

function(proj_test_set_properties TESTNAME)
    set_tests_properties( ${TESTNAME}
        PROPERTIES ENVIRONMENT "PROJ_IGNORE_USER_WRITABLE_DIRECTORY=YES;PROJ_LIB=${PROJECT_BINARY_DIR}/data/for_tests")
endfunction()

function(proj_add_test_script_sh SH_NAME BIN_USE)
  if(UNIX)
    get_filename_component(testname ${SH_NAME} NAME_WE)

    set(TEST_OK 1)
    if(ARGV2)
      set(TEST_OK 0)
      set(GRID_FULLNAME ${PROJECT_SOURCE_DIR}/data/${ARGV2})
      if(EXISTS ${GRID_FULLNAME})
        set(TEST_OK 1)
      endif()
    endif()

    if(${TEST_OK})
      add_test(NAME "${testname}"
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/data
        COMMAND bash ${PROJECT_SOURCE_DIR}/test/cli/${SH_NAME}
        ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${${BIN_USE}}
      )

      proj_test_set_properties(${testname})
    endif()

  endif()
endfunction()


function(proj_add_gie_test TESTNAME TESTCASE)

    set(GIE_BIN $<TARGET_FILE_NAME:gie>)
    set(TESTFILE ${CMAKE_SOURCE_DIR}/test/${TESTCASE})
    add_test(NAME ${TESTNAME}
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
      COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${GIE_BIN}
      ${TESTFILE}
    )
    proj_test_set_properties(${TESTNAME})

endfunction()
