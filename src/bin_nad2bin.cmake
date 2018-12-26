if(WIN32 AND BUILD_LIBPROJ_SHARED)
    message(warning " nad2nad can't be build with a DLL proj library you need a static lib")
endif(WIN32 AND BUILD_LIBPROJ_SHARED)


set(NAD2BIN_SRC apps/nad2bin.cpp)
source_group("Source Files\\Bin" FILES ${NAD2BIN_SRC})

#Executable
add_executable(nad2bin ${NAD2BIN_SRC})
target_link_libraries(nad2bin ${PROJ_LIBRARIES})
install(TARGETS nad2bin 
        RUNTIME DESTINATION ${BINDIR})

if(MSVC AND BUILD_LIBPROJ_SHARED)
    target_compile_definitions(nad2bin PRIVATE PROJ_MSVC_DLL_IMPORT=1)
endif()
