set(PROJ_SRC proj.cpp 
             gen_cheb.cpp 
             p_series.cpp)

source_group("Source Files\\Bin" FILES ${PROJ_SRC})

#Executable
add_executable(binproj ${PROJ_SRC})
set_target_properties(binproj
    PROPERTIES
    OUTPUT_NAME proj)
target_link_libraries(binproj ${PROJ_LIBRARIES})
install(TARGETS binproj 
        RUNTIME DESTINATION ${BINDIR})

if(MSVC AND BUILD_LIBPROJ_SHARED)
    target_compile_definitions(binproj PRIVATE PROJ_MSVC_DLL_IMPORT=1)
endif()
