/* Copyright (c) 2024 Dreamy Cecil
This program is free software; you can redistribute it and/or modify
it under the terms of version 2 of the GNU General Public License as published by
the Free Software Foundation


This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA. */

// [Cecil] NOTE: This is a list of all shader functions from the Shaders project
// This file can be included inline to generate functions, arrays or any other structures
// with all functions in alphabetical order by redefining custom macro functions below

// Define a reference to the dynamic module class by default for creating an external dummy dependency
#ifndef SHADER_FUNC_INLINE
#define SHADER_FUNC_INLINE(_ShaderName) \
  extern DynamicModuleClass Shader_##_ShaderName##_AddToRegistry; \
  void *__##_ShaderName##_Include__ = &Shader_##_ShaderName##_AddToRegistry;
#endif

SHADER_FUNC_INLINE(Add)
SHADER_FUNC_INLINE(AddDS)
SHADER_FUNC_INLINE(Base)
SHADER_FUNC_INLINE(BaseDS)
SHADER_FUNC_INLINE(Base_Transparent)
SHADER_FUNC_INLINE(Base_TransparentDS)
SHADER_FUNC_INLINE(Color)
SHADER_FUNC_INLINE(Detail)
SHADER_FUNC_INLINE(LavaDisplace)
SHADER_FUNC_INLINE(Invisible)
SHADER_FUNC_INLINE(MultiLayer)
SHADER_FUNC_INLINE(Reflection)
SHADER_FUNC_INLINE(ReflectionDS)
SHADER_FUNC_INLINE(ReflectionAndSpecular)
SHADER_FUNC_INLINE(ReflectionAndSpecularDS)
SHADER_FUNC_INLINE(Specular)
SHADER_FUNC_INLINE(SpecularDS)
SHADER_FUNC_INLINE(Translucent)
