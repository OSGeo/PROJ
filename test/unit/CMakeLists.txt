# CMake configuration for PROJ unit tests
# External GTest provided by (e.g.) libgtest-dev

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

  message(STATUS "Fetching GTest")

  # Add Google Test
  #
  # See https://github.com/google/googletest/blob/main/googletest/README.md

  if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)  # for DOWNLOAD_EXTRACT_TIMESTAMP option
  endif()

  include(FetchContent)
  FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/release-1.12.1.zip
    EXCLUDE_FROM_ALL  # ignored before CMake 3.28
  )

  # For Windows: Prevent overriding the parent project's compiler/linker settings
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

  if(CMAKE_VERSION VERSION_GREATER_EQUAL "3.28.0")
    FetchContent_MakeAvailable(googletest)
  else()
    # Pre CMake 3.28 workaround to prevent installing files
    FetchContent_GetProperties(googletest)
    if(NOT googletest_POPULATED)
      FetchContent_Populate(googletest)
      add_subdirectory(${googletest_SOURCE_DIR} ${googletest_BINARY_DIR} EXCLUDE_FROM_ALL)
    endif()
  endif()

endif()  # USE_EXTERNAL_GTEST

#
# Build PROJ unit tests
#

include_directories(${PROJ_SOURCE_DIR}/include)

# Add the directory containing proj_config.h
include_directories(${PROJ_BINARY_DIR}/src)

# Apply to targets in the current directory and below
add_compile_options("$<$<COMPILE_LANGUAGE:C>:${PROJ_C_WARN_FLAGS}>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${PROJ_CXX_WARN_FLAGS}>")

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

set(PROJ_TEST_CPP_API_SOURCES
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
add_executable(proj_test_cpp_api ${PROJ_TEST_CPP_API_SOURCES})
set_property(SOURCE ${PROJ_TEST_CPP_API_SOURCES} PROPERTY SKIP_UNITY_BUILD_INCLUSION ON)

target_link_libraries(proj_test_cpp_api
  PRIVATE GTest::gtest
  PRIVATE SQLite::SQLite3
  PRIVATE ${PROJ_LIBRARIES})
add_test(NAME proj_test_cpp_api COMMAND proj_test_cpp_api)
set_property(TEST proj_test_cpp_api
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})
if(TIFF_ENABLED)
  target_compile_definitions(proj_test_cpp_api PRIVATE -DTIFF_ENABLED)
endif()
 
add_executable(gie_self_tests
  main.cpp
  gie_self_tests.cpp)
target_link_libraries(gie_self_tests
  PRIVATE GTest::gtest
  PRIVATE ${PROJ_LIBRARIES})
  
add_test(NAME gie_self_tests COMMAND gie_self_tests)
set_property(TEST gie_self_tests
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})
if(TIFF_ENABLED)
  target_compile_definitions(gie_self_tests PRIVATE -DTIFF_ENABLED)
endif()

add_executable(test_network
  main.cpp
  test_network.cpp)
if(CURL_ENABLED)
  target_compile_definitions(test_network PRIVATE -DCURL_ENABLED)
  target_link_libraries(test_network PRIVATE CURL::libcurl)
endif()
target_link_libraries(test_network
  PRIVATE GTest::gtest
  PRIVATE SQLite::SQLite3
  PRIVATE ${PROJ_LIBRARIES})
if(TIFF_ENABLED)
add_test(NAME test_network COMMAND test_network)
set_property(TEST test_network
  PROPERTY ENVIRONMENT ${PROJ_TEST_ENVIRONMENT})
endif()

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
