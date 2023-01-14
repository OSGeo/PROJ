# CMake configuration for PROJ unit tests

find_package(GTest 1.8.1)
if(GTest_FOUND)
  option(USE_EXTERNAL_GTEST "Compile against external GTest" ON)
else()
  option(USE_EXTERNAL_GTEST "Compile against external GTest" OFF)
endif()

if(USE_EXTERNAL_GTEST)

if(NOT GTest_FOUND)
    message(FATAL_ERROR "External GTest >= 1.8.1 not found")
endif()
message(STATUS "Using external GTest")

# CMake < 3.20.0 uses GTest::GTest
# CMake >= 3.20 uses GTest::gtest, and deprecates GTest::GTest
# so for older CMake, create an alias from GTest::GTest to GTest::gtest
if(NOT TARGET GTest::gtest)
    add_library(GTest::gtest INTERFACE IMPORTED)
    set_target_properties(GTest::gtest PROPERTIES
                          INTERFACE_LINK_LIBRARIES "GTest::GTest")
endif()

else()

message(STATUS "Using internal GTest")

#
# Build Google Test
#
# Source https://github.com/google/googletest/blob/master/googletest/README.md
# Download and unpack googletest at configure time
configure_file(
  ${PROJ_SOURCE_DIR}/test/googletest/CMakeLists.txt.in
  ${PROJ_BINARY_DIR}/googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${PROJ_BINARY_DIR}/googletest-download)
if(result)
  message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${PROJ_BINARY_DIR}/googletest-download)
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
  ${PROJ_BINARY_DIR}/googletest-src
  ${PROJ_BINARY_DIR}/googletest-build
  EXCLUDE_FROM_ALL)

# Provide the same target name as find_package(GTest)
add_library(GTest::gtest ALIAS gtest)

endif()  # USE_EXTERNAL_GTEST

#
# Build PROJ unit tests
#

include_directories(${PROJ_SOURCE_DIR}/include)
include_directories(${SQLITE3_INCLUDE_DIR})
# Add the directory containing proj_config.h
include_directories(${PROJ_BINARY_DIR}/src)

# Apply to targets in the current directory and below
add_compile_options(${PROJ_CXX_WARN_FLAGS})

set(PROJ_TEST_ENVIRONMENT
  "PROJ_SKIP_READ_USER_WRITABLE_DIRECTORY=YES"
  "PROJ_DATA=${PROJ_BINARY_DIR}/data/for_tests"
  "PROJ_SOURCE_DATA=${PROJ_SOURCE_DIR}/data"
)

add_executable(proj_errno_string_test
  main.cpp
  proj_errno_string_test.cpp)
target_link_libraries(proj_errno_string_test
  PRIVATE GTest::gtest
  PRIVATE ${PROJ_LIBRARIES})
add_test(NAME proj_errno_string_test COMMAND proj_errno_string_test)
set_property(TEST proj_errno_string_test
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})

add_executable(proj_angular_io_test
  main.cpp
  proj_angular_io_test.cpp)
target_link_libraries(proj_angular_io_test
  PRIVATE GTest::gtest
  PRIVATE ${PROJ_LIBRARIES})
add_test(NAME proj_angular_io_test COMMAND proj_angular_io_test)
set_property(TEST proj_angular_io_test
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})

add_executable(proj_context_test
  main.cpp
  proj_context_test.cpp)
target_link_libraries(proj_context_test
  PRIVATE GTest::gtest
  PRIVATE ${PROJ_LIBRARIES})
add_test(NAME proj_context_test COMMAND proj_context_test)
set_property(TEST proj_context_test
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})

if(MSVC AND BUILD_SHARED_LIBS)
  # ph_phi2_test not compatible of a .dll build
else()
  add_executable(pj_phi2_test
    main.cpp
    pj_phi2_test.cpp)
  target_link_libraries(pj_phi2_test
    PRIVATE GTest::gtest
    PRIVATE ${PROJ_LIBRARIES})
  add_test(NAME pj_phi2_test COMMAND pj_phi2_test)
  set_property(TEST pj_phi2_test
    PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})
endif()

add_executable(proj_test_cpp_api
  main.cpp
  test_util.cpp
  test_common.cpp
  test_coordinates.cpp
  test_crs.cpp
  test_metadata.cpp
  test_io.cpp
  test_operation.cpp
  test_operationfactory.cpp
  test_datum.cpp
  test_factory.cpp
  test_c_api.cpp
  test_grids.cpp)
target_link_libraries(proj_test_cpp_api
  PRIVATE GTest::gtest
  PRIVATE ${PROJ_LIBRARIES}
  PRIVATE ${SQLITE3_LIBRARY})
add_test(NAME proj_test_cpp_api COMMAND proj_test_cpp_api)
set_property(TEST proj_test_cpp_api
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})

add_executable(gie_self_tests
  main.cpp
  gie_self_tests.cpp)
target_link_libraries(gie_self_tests
  PRIVATE GTest::gtest
  PRIVATE ${PROJ_LIBRARIES})
add_test(NAME gie_self_tests COMMAND gie_self_tests)
set_property(TEST gie_self_tests
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})

add_executable(test_network
  main.cpp
  test_network.cpp)
if(CURL_ENABLED)
  target_compile_definitions(test_network PRIVATE -DCURL_ENABLED)
  if( CMAKE_VERSION VERSION_LESS 3.11 AND CMAKE_CROSSCOMPILING )
      # Hack needed for ubuntu:18.04 mingw64 cross compiling to avoid
      # -isystem to be emitted (similar to https://discourse.cmake.org/t/use-of-isystem/1574)
      target_include_directories(test_network PRIVATE ${CURL_INCLUDE_DIRS})
      target_link_libraries(test_network PRIVATE ${CURL_LIBRARIES})
  else()
      target_link_libraries(test_network PRIVATE CURL::libcurl)
  endif()
endif()
target_link_libraries(test_network
  PRIVATE GTest::gtest
  PRIVATE ${PROJ_LIBRARIES}
  PRIVATE ${SQLITE3_LIBRARY})
add_test(NAME test_network COMMAND test_network)
set_property(TEST test_network
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})

add_executable(test_defmodel
  main.cpp
  test_defmodel.cpp)
target_link_libraries(test_defmodel
  PRIVATE GTest::gtest
  PRIVATE ${PROJ_LIBRARIES})
add_test(NAME test_defmodel COMMAND test_defmodel)
set_property(TEST test_defmodel
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})

add_executable(test_tinshift
  main.cpp
  test_tinshift.cpp)
target_link_libraries(test_tinshift
  PRIVATE GTest::gtest
  PRIVATE ${PROJ_LIBRARIES})
add_test(NAME test_tinshift COMMAND test_tinshift)
set_property(TEST test_tinshift
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})

add_executable(test_misc
  main.cpp
  test_misc.cpp)
target_link_libraries(test_misc
  PRIVATE GTest::gtest
  PRIVATE ${PROJ_LIBRARIES})
add_test(NAME test_misc COMMAND test_misc)
set_property(TEST test_misc
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})

if (Threads_FOUND AND CMAKE_USE_PTHREADS_INIT)
add_definitions(-DPROJ_HAS_PTHREADS)
add_executable(test_fork
  test_fork.c)
target_link_libraries(test_fork
  PRIVATE ${PROJ_LIBRARIES}
  PRIVATE ${CMAKE_THREAD_LIBS_INIT})
add_test(NAME test_fork COMMAND test_fork)
set_property(TEST test_fork
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})
endif()
