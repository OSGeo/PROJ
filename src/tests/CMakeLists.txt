# Build PROJ test programs and run tests (only invoked if BUILD_TESTING)

add_executable(geodtest geodtest.c)
target_link_libraries(geodtest ${PROJ_LIBRARIES})

add_executable(geodsigntest geodsigntest.c)
# geodsigntest includes geodesic.c directly so it doesn't need to link
# against the library.
target_link_libraries(geodsigntest m)

# Do not install, instead run tests
add_test(NAME geodesic-test COMMAND geodtest)
add_test(NAME geodesic-signtest COMMAND geodsigntest)
