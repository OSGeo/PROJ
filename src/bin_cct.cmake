set(CCT_SRC cct.c proj_strtod.c proj_strtod.h)
set(CCT_INCLUDE optargpm.h)

source_group("Source Files\\Bin" FILES ${CCT_SRC})

add_executable(cct ${CCT_SRC} ${CCT_INCLUDE})
target_link_libraries(cct ${PROJ_LIBRARIES})
install(TARGETS cct
	RUNTIME DESTINATION ${BINDIR})
