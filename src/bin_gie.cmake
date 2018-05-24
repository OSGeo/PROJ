set(GIE_SRC gie.c proj_strtod.c proj_strtod.h)
set(GIE_INCLUDE optargpm.h)

source_group("Source Files\\Bin" FILES ${GIE_SRC})

add_executable(gie ${GIE_SRC} ${GIE_INCLUDE})
target_link_libraries(gie ${PROJ_LIBRARIES})
install(TARGETS gie
	RUNTIME DESTINATION ${BINDIR})
