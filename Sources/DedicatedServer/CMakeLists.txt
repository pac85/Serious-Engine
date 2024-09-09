cmake_minimum_required(VERSION 3.7.2)
project(DedicatedServer)

add_executable(DedicatedServer "DedicatedServer.cpp")
add_dependencies(DedicatedServer ${GAMELIB} Engine)

target_link_libraries(DedicatedServer Engine ${ENTITIESLIB} ${GAMELIB} ${SHADERSLIB})
#set_target_properties(DedicatedServer PROPERTIES ENABLE_EXPORTS ON)

if(LINUX)
  set_target_properties(DedicatedServer PROPERTIES LINK_FLAGS "-Wl,-rpath,$ORIGIN")
  target_link_libraries(DedicatedServer "m")
  target_link_libraries(DedicatedServer "dl")
  target_link_libraries(DedicatedServer "pthread")
  target_link_libraries(DedicatedServer ${SDL2_LIBRARY} ${ZLIB_LIBRARIES})
endif()
