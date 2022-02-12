set(GEODTEST_SRC geodtest.cpp)
set(GEODTEST_INCLUDE)

source_group("Source Files\\Bin" FILES ${GEODTEST_SRC} ${GEODTEST_INCLUDE})

add_executable(geodtest ${GEODTEST_SRC} ${GEODTEST_INCLUDE})
target_link_libraries(geodtest PRIVATE ${PROJ_LIBRARIES})

# Do not install, instead run as a test
add_test(NAME geodesic-test COMMAND geodtest)
