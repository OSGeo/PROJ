set(GEOD_SRC
  geod.cpp
  geod_set.cpp
  geod_interface.cpp
  emess.cpp
  utils.cpp
)
set(GEOD_INCLUDE geod_interface.h)

source_group("Source Files\\Bin" FILES ${GEOD_SRC} ${GEOD_INCLUDE})

add_executable(geod ${GEOD_SRC} ${GEOD_INCLUDE})
target_link_libraries(geod PRIVATE ${PROJ_LIBRARIES})

install(TARGETS geod
  DESTINATION ${CMAKE_INSTALL_BINDIR})

# invgeod target: symlink or copy of geod executable

if(UNIX)

    set(link_target "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/invgeod${CMAKE_EXECUTABLE_SUFFIX}")
    set(link_source "geod${CMAKE_EXECUTABLE_SUFFIX}")

    add_custom_command(
      OUTPUT ${link_target}
      COMMAND ${CMAKE_COMMAND} -E create_symlink ${link_source} ${link_target}
      WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
      DEPENDS geod
      COMMENT "Generating invgeod"
      VERBATIM
    )

    add_custom_target(invgeod ALL DEPENDS ${link_target})

    install(FILES ${link_target} DESTINATION ${CMAKE_INSTALL_BINDIR})

else()

    add_executable(invgeod ${GEOD_SRC} ${GEOD_INCLUDE})
    target_link_libraries(invgeod PRIVATE ${PROJ_LIBRARIES})

    install(TARGETS invgeod
      DESTINATION ${CMAKE_INSTALL_BINDIR})

endif()
