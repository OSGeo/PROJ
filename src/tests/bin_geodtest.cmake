set(GEODTEST_SRC geodtest.cpp)
set(GEODTEST_INCLUDE)

source_group("Source Files\\Bin" FILES ${GEODTEST_SRC} ${GEODTEST_INCLUDE})

add_executable(geodtest ${GEODTEST_SRC} ${GEODTEST_INCLUDE})
target_link_libraries(geodtest PRIVATE ${PROJ_LIBRARIES})

# Do not install, instead run as a test
add_test(NAME geodesic-test COMMAND geodtest)

set(GEODSIGNTEST_SRC geodsigntest.cpp)
set(GEODSIGNTEST_INCLUDE)

source_group("Source Files\\Bin" FILES ${GEODSIGNTEST_SRC} ${GEODSIGNTEST_INCLUDE})

add_executable(geodsigntest ${GEODSIGNTEST_SRC} ${GEODSIGNTEST_INCLUDE})
# geodsigntest includes geodesic.c directly so it doesn't need to link
# against the library.

# Do not install, instead run as a test
add_test(NAME geodesic-signtest COMMAND geodsigntest)
