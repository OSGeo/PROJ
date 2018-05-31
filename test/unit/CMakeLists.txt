#SET(CATCH2_INCLUDE ../catch.hpp)
#include_directories(..)

#SET(BASIC_TEST_SRC basic_test.cpp)
#add_executable(basic_test ${BASIC_TEST_SRC} ${CATCH2_INCLUDE})
#target_link_libraries(basic_test ${PROJ_LIBRARIES} ${TEST_MAIN_LIBRARIES})
#set_target_properties(basic_test
#    PROPERTIES
#    LINKER_LANGUAGE CXX)
#
#add_test( NAME basic_test
#            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
#            COMMAND basic_test )
