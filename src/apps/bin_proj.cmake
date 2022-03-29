set(PROJ_SRC
  proj.cpp
  emess.cpp
  utils.cpp
)

source_group("Source Files\\Bin" FILES ${PROJ_SRC})

add_executable(binproj ${PROJ_SRC})
set_target_properties(binproj
  PROPERTIES
  RUNTIME_OUTPUT_NAME proj)
target_link_libraries(binproj PRIVATE ${PROJ_LIBRARIES})

install(TARGETS binproj
  DESTINATION ${CMAKE_INSTALL_BINDIR})

# invproj target: symlink or copy of proj executable

if(UNIX)

    set(link_target "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/invproj${CMAKE_EXECUTABLE_SUFFIX}")
    set(link_source "proj${CMAKE_EXECUTABLE_SUFFIX}")

    add_custom_command(
      OUTPUT ${link_target}
      COMMAND ${CMAKE_COMMAND} -E create_symlink ${link_source} ${link_target}
      WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
      DEPENDS binproj
      COMMENT "Generating invproj"
      VERBATIM
    )

    add_custom_target(invproj ALL DEPENDS ${link_target})

    install(FILES ${link_target} DESTINATION ${CMAKE_INSTALL_BINDIR})

else()

    add_executable(invproj ${PROJ_SRC})
    target_link_libraries(invproj PRIVATE ${PROJ_LIBRARIES})

    install(TARGETS invproj
      DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()
