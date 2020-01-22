# CMake configuration for PROJ unit tests

option(USE_EXTERNAL_GTEST "Compile against external GTest" OFF)

if(USE_EXTERNAL_GTEST)

message(STATUS "Using external GTest")
find_package(GTest 1.8.1 CONFIG REQUIRED)

else()

message(STATUS "Using internal GTest")

#
# Build Google Test
#
# Source https://github.com/google/googletest/blob/master/googletest/README.md
# Download and unpack googletest at configure time
configure_file(
  ${CMAKE_SOURCE_DIR}/test/googletest/CMakeLists.txt.in
  ${CMAKE_BINARY_DIR}/googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/googletest-download)
if(result)
  message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()
# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
option(INSTALL_GTEST "Enable installation of googletest" OFF)
add_subdirectory(
  ${CMAKE_BINARY_DIR}/googletest-src
  ${CMAKE_BINARY_DIR}/googletest-build
  EXCLUDE_FROM_ALL)

# Provide the same target name as find_package(GTest)
add_library(GTest::gtest ALIAS gtest)

endif()  # USE_EXTERNAL_GTEST

#
# Build PROJ unit tests
#

if("${CMAKE_C_COMPILER_ID}" STREQUAL "MSVC" AND BUILD_LIBPROJ_SHARED)
  add_definitions(-DPROJ_MSVC_DLL_IMPORT=1)
endif()

include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${SQLITE3_INCLUDE_DIR})
# Add the directory containing proj_config.h
include_directories(${CMAKE_BINARY_DIR}/src)

# Apply to targets in the current directory and below
add_compile_options(${PROJ_CXX_WARN_FLAGS})

add_executable(proj_pj_transform_test
  main.cpp
  pj_transform_test.cpp)
target_link_libraries(proj_pj_transform_test
  GTest::gtest
  ${PROJ_LIBRARIES})
add_test(NAME proj_pj_transform_test COMMAND proj_pj_transform_test)
set_property(TEST proj_pj_transform_test
        PROPERTY ENVIRONMENT "PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES;PROJ_LIB=${PROJECT_BINARY_DIR}/data/for_tests")


add_executable(proj_errno_string_test
  main.cpp
  proj_errno_string_test.cpp)
target_link_libraries(proj_errno_string_test
  GTest::gtest
  ${PROJ_LIBRARIES})
add_test(NAME proj_errno_string_test COMMAND proj_errno_string_test)
set_property(TEST proj_errno_string_test
        PROPERTY ENVIRONMENT "PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES;PROJ_LIB=${PROJECT_BINARY_DIR}/data/for_tests")

add_executable(proj_angular_io_test
  main.cpp
  proj_angular_io_test.cpp)
target_link_libraries(proj_angular_io_test
  GTest::gtest
  ${PROJ_LIBRARIES})
add_test(NAME proj_angular_io_test COMMAND proj_angular_io_test)
set_property(TEST proj_angular_io_test
        PROPERTY ENVIRONMENT "PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES;PROJ_LIB=${PROJECT_BINARY_DIR}/data/for_tests")

add_executable(proj_context_test
  main.cpp
  proj_context_test.cpp)
target_link_libraries(proj_context_test
  GTest::gtest
  ${PROJ_LIBRARIES})
add_test(NAME proj_context_test COMMAND proj_context_test)
set_property(TEST proj_context_test
        PROPERTY ENVIRONMENT "PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES;PROJ_LIB=${PROJECT_BINARY_DIR}/data/for_tests")

if(MSVC AND BUILD_LIBPROJ_SHARED)
  # ph_phi2_test not compatible of a .dll build
else()
  add_executable(pj_phi2_test
    main.cpp
    pj_phi2_test.cpp)
  target_link_libraries(pj_phi2_test
    GTest::gtest
    ${PROJ_LIBRARIES})
  add_test(NAME pj_phi2_test COMMAND pj_phi2_test)
  set_property(TEST pj_phi2_test
        PROPERTY ENVIRONMENT "PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES;PROJ_LIB=${PROJECT_BINARY_DIR}/data/for_tests")
endif()

add_executable(proj_test_cpp_api
  main.cpp
  test_util.cpp
  test_common.cpp
  test_crs.cpp
  test_metadata.cpp
  test_io.cpp
  test_operation.cpp
  test_datum.cpp
  test_factory.cpp
  test_c_api.cpp
  test_grids.cpp)
target_link_libraries(proj_test_cpp_api
  GTest::gtest
  ${PROJ_LIBRARIES}
  ${SQLITE3_LIBRARY})
add_test(NAME proj_test_cpp_api COMMAND proj_test_cpp_api)
set_property(TEST proj_test_cpp_api
        PROPERTY ENVIRONMENT "PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES;PROJ_LIB=${PROJECT_BINARY_DIR}/data/for_tests")


add_executable(gie_self_tests
  main.cpp
  gie_self_tests.cpp)
target_link_libraries(gie_self_tests
  GTest::gtest
  ${PROJ_LIBRARIES})
add_test(NAME gie_self_tests COMMAND gie_self_tests)
set_property(TEST gie_self_tests
        PROPERTY ENVIRONMENT "PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES;PROJ_LIB=${PROJECT_BINARY_DIR}/data/for_tests")


add_executable(test_network
  main.cpp
  test_network.cpp)
if(CURL_FOUND)
  include_directories(${CURL_INCLUDE_DIR})
  target_link_libraries(test_network ${CURL_LIBRARY})
endif()
target_link_libraries(test_network
  GTest::gtest
  ${PROJ_LIBRARIES}
  ${SQLITE3_LIBRARY})
add_test(NAME test_network COMMAND test_network)
set_property(TEST test_network
        PROPERTY ENVIRONMENT "PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES;PROJ_LIB=${PROJECT_BINARY_DIR}/data/for_tests;PROJ_SOURCE_DATA=${PROJECT_SOURCE_DIR}/data")
