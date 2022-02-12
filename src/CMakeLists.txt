# Global compile options for current directory and subdirectories
add_compile_options("$<$<COMPILE_LANGUAGE:C>:${PROJ_C_WARN_FLAGS}>")
add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${PROJ_CXX_WARN_FLAGS}>")

# First configure proj library
include(lib_proj.cmake)
add_subdirectory(apps)
add_subdirectory(tests)

if(APPLE)
  # Directory name for installed targets
  set(CMAKE_INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/${LIBDIR}")
endif()
