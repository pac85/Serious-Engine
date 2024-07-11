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

// [Cecil] NOTE: This is a list of all entity classes from the Engine project
// This file can be included inline to generate functions, arrays or any other structures
// with all classes in alphabetical order by redefining custom macro functions below

// Define a reference to the dynamic module class by default for creating an external dummy dependency
#ifndef ES_CLASS_INLINE
#define ES_CLASS_INLINE(_Class) \
  extern DynamicModuleClass _Class##_AddToRegistry; \
  void *__##_Class##_Include__ = &_Class##_AddToRegistry;
#endif

ES_CLASS_INLINE(CEntity)
ES_CLASS_INLINE(CLiveEntity)
ES_CLASS_INLINE(CMovableBrushEntity)
ES_CLASS_INLINE(CMovableEntity)
ES_CLASS_INLINE(CMovableModelEntity)
ES_CLASS_INLINE(CPlayerEntity)
ES_CLASS_INLINE(CRationalEntity)
