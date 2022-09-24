# Build PROJ test programs and run tests (only invoked if BUILD_TESTING)

add_executable(geodtest geodtest.c)
target_link_libraries(geodtest ${PROJ_LIBRARIES})
if(HAVE_LIBM)
  target_link_libraries(geodtest m)
endif()

add_executable(geodsigntest geodsigntest.c)
# geodsigntest includes geodesic.c directly so it doesn't need to link
# against the library.
if(HAVE_LIBM)
  target_link_libraries(geodsigntest m)
endif()

# Do not install, instead run tests
add_test(NAME geodesic-test COMMAND geodtest)
add_test(NAME geodesic-signtest COMMAND geodsigntest)
