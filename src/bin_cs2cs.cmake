set(CS2CS_SRC cs2cs.c 
              gen_cheb.c 
              p_series.c)

source_group("Source Files\\Bin" FILES ${CS2CS_SRC})

add_executable(cs2cs ${CS2CS_SRC} ${CS2CS_INCLUDE})
target_link_libraries(cs2cs ${PROJ_LIBRARIES})
install(TARGETS cs2cs 
        RUNTIME DESTINATION ${BINDIR})

if(MSVC AND BUILD_LIBPROJ_SHARED)
    target_compile_definitions(cs2cs PRIVATE PROJ_MSVC_DLL_IMPORT=1)
endif()
