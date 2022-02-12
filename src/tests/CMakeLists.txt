# BUILD_GEOD is defined in src/apps/CMakeLists.txt
if(BUILD_TESTING AND BUILD_GEOD)
  include(bin_geodtest.cmake)
endif()
