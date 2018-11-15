set(GEOD_SRC geod.c
             geod_set.c geod_interface.c )
set(GEOD_INCLUDE geod_interface.h)

source_group("Source Files\\Bin" FILES ${GEOD_SRC} ${GEOD_INCLUDE})

#Executable
add_executable(geod ${GEOD_SRC} ${GEOD_INCLUDE})
target_link_libraries(geod ${PROJ_LIBRARIES})
install(TARGETS geod
        RUNTIME DESTINATION ${BINDIR})

if(MSVC AND BUILD_LIBPROJ_SHARED)
    target_compile_definitions(geod PRIVATE PROJ_MSVC_DLL_IMPORT=1)
endif()
