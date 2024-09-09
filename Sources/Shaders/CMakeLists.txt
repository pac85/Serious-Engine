cmake_minimum_required(VERSION 3.7.2)
project(Shaders)

set(SHADERS_SRCS
  "AddShader.cpp"
  "AddShaderDS.cpp"
  "BaseShader.cpp"
  "BaseShaderDS.cpp"
  "BaseTransparent.cpp"
  "BaseTransparentDS.cpp"
  "ColorShader.cpp"
  "Common.cpp"
  "DetailShader.cpp"
  "DisplaceShader.cpp"
  "InvisibleShader.cpp"
  "MultiLayerShader.cpp"
  "Reflection.cpp"
  "ReflectionDS.cpp"
  "ReftectionAndSpecular.cpp"
  "ReftectionAndSpecularDS.cpp"
  "Specular.cpp"
  "SpecularDS.cpp"
  "StdH.cpp"
  "Translucent.cpp"
)

add_library(${SHADERSLIB} STATIC ${SHADERS_SRCS})
add_dependencies(${SHADERSLIB} Engine)

target_link_libraries(${SHADERSLIB} Engine)
set_target_properties(${SHADERSLIB} PROPERTIES ENABLE_EXPORTS ON)
