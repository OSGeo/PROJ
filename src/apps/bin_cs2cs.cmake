set(CS2CS_SRC
  cs2cs.cpp
  emess.cpp
  utils.cpp
)

source_group("Source Files\\Bin" FILES ${CS2CS_SRC})

add_executable(cs2cs ${CS2CS_SRC} ${CS2CS_INCLUDE})
target_link_libraries(cs2cs PRIVATE ${PROJ_LIBRARIES})

install(TARGETS cs2cs
  DESTINATION ${CMAKE_INSTALL_BINDIR})
