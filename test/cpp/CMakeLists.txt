SET(CATCH2_INCLUDE catch.hpp)

SET(TEST_MAIN_SRC test_main.cpp)
set(TEST_MAIN_LIBRARIES test_main)
add_library( ${TEST_MAIN_LIBRARIES}
                    ${PROJ_LIBRARY_TYPE}
                    ${TEST_MAIN_SRC}
                    ${CATCH2_INCLUDE}  )

SET(BASIC_TEST_SRC basic_test.cpp)
add_executable(basic_test ${BASIC_TEST_SRC} ${CATCH2_INCLUDE})
target_link_libraries(basic_test ${PROJ_LIBRARIES} ${TEST_MAIN_LIBRARIES})
set_target_properties(basic_test
    PROPERTIES
    LINKER_LANGUAGE CXX)

add_test( NAME basic_test
            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
            COMMAND ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/basic_test )
