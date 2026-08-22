cmake_minimum_required(VERSION 3.22.1)
project(CPP_APP LANGUAGES CXX)

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
target_compile_features(cpp_app
  PRIVATE
    cxx_std_11
)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/..)
include(common)
add_test_libpath(cpp_app)
add_test_transform(cpp_app)
add_test_searchpath(cpp_app)
add_test_version(cpp_app)
