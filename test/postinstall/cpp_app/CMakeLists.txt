cmake_minimum_required(VERSION 3.5)
project(CPP_APP LANGUAGES CXX)

# Required for (e.g.) g++-4.8
# can be refactored for CMake 3.22
# https://github.com/OSGeo/PROJ/issues/1924
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(USE_PROJ_NAME "PROJ"
  CACHE STRING "Either PROJ (default) or PROJ4")

find_package(${USE_PROJ_NAME} REQUIRED CONFIG)

include(CMakePrintHelpers)
cmake_print_properties(
  TARGETS ${USE_PROJ_NAME}::proj
  PROPERTIES
    LOCATION
    INTERFACE_INCLUDE_DIRECTORIES
    INTERFACE_LINK_LIBRARIES
    INTERFACE_COMPILE_FEATURES
)

add_executable(cpp_app cpp_app.cpp)
target_link_libraries(cpp_app PRIVATE ${USE_PROJ_NAME}::proj)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/..)
include(common)
add_test_libpath(cpp_app)
add_test_transform(cpp_app)
add_test_searchpath(cpp_app)
add_test_version(cpp_app)
