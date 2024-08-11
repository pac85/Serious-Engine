/* Copyright (c) 2002-2012 Croteam Ltd.
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

#ifndef EAX_ENVTYPES_H_INCLUDED
#define EAX_ENVTYPES_H_INCLUDED

// [Cecil] Moved out of eax.h to be able to use them without EAX support
enum {
  EAX_ENVIRONMENT_GENERIC,
  EAX_ENVIRONMENT_PADDEDCELL,
  EAX_ENVIRONMENT_ROOM,
  EAX_ENVIRONMENT_BATHROOM,
  EAX_ENVIRONMENT_LIVINGROOM,
  EAX_ENVIRONMENT_STONEROOM,
  EAX_ENVIRONMENT_AUDITORIUM,
  EAX_ENVIRONMENT_CONCERTHALL,
  EAX_ENVIRONMENT_CAVE,
  EAX_ENVIRONMENT_ARENA,
  EAX_ENVIRONMENT_HANGAR,
  EAX_ENVIRONMENT_CARPETEDHALLWAY,
  EAX_ENVIRONMENT_HALLWAY,
  EAX_ENVIRONMENT_STONECORRIDOR,
  EAX_ENVIRONMENT_ALLEY,
  EAX_ENVIRONMENT_FOREST,
  EAX_ENVIRONMENT_CITY,
  EAX_ENVIRONMENT_MOUNTAINS,
  EAX_ENVIRONMENT_QUARRY,
  EAX_ENVIRONMENT_PLAIN,
  EAX_ENVIRONMENT_PARKINGLOT,
  EAX_ENVIRONMENT_SEWERPIPE,
  EAX_ENVIRONMENT_UNDERWATER,
  EAX_ENVIRONMENT_DRUGGED,
  EAX_ENVIRONMENT_DIZZY,
  EAX_ENVIRONMENT_PSYCHOTIC,

  EAX_ENVIRONMENT_COUNT
};

#endif
