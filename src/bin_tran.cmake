set(TRAN_SRC tran.c)

source_group("Source Files\\Bin" FILES ${TRAN_SRC})

if(WIN32)
    set(TRAN_SRC ${TRAN_SRC} emess.c)
endif(WIN32)
    
#Executable
add_executable(bintran ${TRAN_SRC})
set_target_properties(bintran
    PROPERTIES
    OUTPUT_NAME tran)
target_link_libraries(bintran ${PROJ_LIBRARIES})
install(TARGETS bintran 
        RUNTIME DESTINATION ${BINDIR})
		