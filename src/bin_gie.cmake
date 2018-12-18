set(GIE_SRC gie.cpp proj_strtod.cpp proj_strtod.h)
set(GIE_INCLUDE optargpm.h)

source_group("Source Files\\Bin" FILES ${GIE_SRC})

add_executable(gie ${GIE_SRC} ${GIE_INCLUDE})
target_link_libraries(gie ${PROJ_LIBRARIES})
install(TARGETS gie
	RUNTIME DESTINATION ${BINDIR})

if(MSVC AND BUILD_LIBPROJ_SHARED)
    target_compile_definitions(gie PRIVATE PROJ_MSVC_DLL_IMPORT=1)
endif()
