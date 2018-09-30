set(PROJINFO_SRC projinfo.cpp)

source_group("Source Files\\Bin" FILES ${PROJINFO_SRC})

#Executable
add_executable(binprojinfo ${PROJINFO_SRC})
set_target_properties(binprojinfo
    PROPERTIES
    OUTPUT_NAME projinfo)
target_link_libraries(binprojinfo ${PROJ_LIBRARIES})
install(TARGETS binprojinfo 
        RUNTIME DESTINATION ${BINDIR})

