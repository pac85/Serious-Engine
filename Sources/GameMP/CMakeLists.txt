cmake_minimum_required(VERSION 3.7.2)
project(GameMP)

set(GAMELIB_SRCS
  "Camera.cpp"
  "CompMessage.cpp"
  "CompModels.cpp"
  "Computer.cpp"
  "Console.cpp"
  "Controls.cpp"
  "Game.cpp"
  "Interface.cpp"
  "LCDDrawing.cpp"
  "LoadingHook.cpp"
  "Map.cpp"
  "SessionProperties.cpp"
)

add_library(${GAMELIB} STATIC ${GAMELIB_SRCS})
add_dependencies(${GAMELIB} Engine ${ENTITIESLIB})

target_link_libraries(${GAMELIB} Engine ${ENTITIESLIB})
set_target_properties(${GAMELIB} PROPERTIES ENABLE_EXPORTS ON)
