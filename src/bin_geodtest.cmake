set(GEODTEST_SRC geodtest.cpp )
set(GEODTEST_INCLUDE)

source_group("Source Files\\Bin" FILES ${GEODTEST_SRC} ${GEODTEST_INCLUDE})

#Executable
add_executable(geodtest ${GEODTEST_SRC} ${GEODTEST_INCLUDE})
target_link_libraries(geodtest ${PROJ_LIBRARIES})
# Do not install

# Instead run as a test
add_test (NAME geodesic-test COMMAND geodtest)

if(MSVC AND BUILD_LIBPROJ_SHARED)
    target_compile_definitions(geodtest PRIVATE PROJ_MSVC_DLL_IMPORT=1)
endif()
