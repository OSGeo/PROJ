set(CCT_SRC cct.cpp proj_strtod.cpp proj_strtod.h)
set(CCT_INCLUDE optargpm.h)

source_group("Source Files\\Bin" FILES ${CCT_SRC})

add_executable(cct ${CCT_SRC} ${CCT_INCLUDE})
target_link_libraries(cct ${PROJ_LIBRARIES})
install(TARGETS cct
	RUNTIME DESTINATION ${BINDIR})

if(MSVC AND BUILD_LIBPROJ_SHARED)
    target_compile_definitions(cct PRIVATE PROJ_MSVC_DLL_IMPORT=1)
endif()
