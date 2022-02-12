set(PROJSYNC_SRC projsync.cpp)

source_group("Source Files\\Bin" FILES ${PROJSYNC_SRC})

add_executable(projsync ${PROJSYNC_SRC})
target_link_libraries(projsync PRIVATE ${PROJ_LIBRARIES})

install(TARGETS projsync
  DESTINATION ${BINDIR})
