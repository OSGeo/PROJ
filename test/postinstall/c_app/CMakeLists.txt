cmake_minimum_required(VERSION 3.5)
project(C_APP LANGUAGES C)

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

add_executable(c_app c_app.c)
target_link_libraries(c_app PRIVATE ${USE_PROJ_NAME}::proj)

get_target_property(PROJLIB_LOCATION ${USE_PROJ_NAME}::proj LOCATION)
if(PROJLIB_LOCATION MATCHES ".*\.(lib|a)$")
  # Used for static linking (is there a better way?)
  enable_language(CXX)
  set_target_properties(c_app PROPERTIES LINKER_LANGUAGE CXX)
endif()

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/..)
include(common)
add_test_libpath(c_app proj)
add_test_transform(c_app)
add_test_searchpath(c_app)
add_test_version(c_app)
