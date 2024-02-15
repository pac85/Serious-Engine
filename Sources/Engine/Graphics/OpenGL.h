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

#ifndef SE_INCL_OPENGL_H
#define SE_INCL_OPENGL_H
#ifdef PRAGMA_ONCE
  #pragma once
#endif

#if !SE1_GLEW
  #include "gl_types.h"
#endif

/* rcg10042001 wraped for platform. */
#if SE1_WIN
  #define DLLFUNCTION(dll, output, name, inputs, params, required) \
    extern output (__stdcall *p##name) inputs

#elif SE1_UNIX
  #define DLLFUNCTION(dll, output, name, inputs, params, required) \
    extern output (*p##name) inputs
    #define __stdcall

#else
  #error please define your platform here.
#endif

#include "gl_functions.h"

#undef DLLFUNCTION

// extensions
extern void (__stdcall *pglLockArraysEXT)(GLint first, GLsizei count);
extern void (__stdcall *pglUnlockArraysEXT)(void);

#if !SE1_PREFER_SDL
extern GLboolean (__stdcall *pwglSwapIntervalEXT)(GLint interval);
extern GLint     (__stdcall *pwglGetSwapIntervalEXT)(void);
#endif

extern void (__stdcall *pglActiveTextureARB)(GLenum texunit);
extern void (__stdcall *pglClientActiveTextureARB)(GLenum texunit);

#if !SE1_PREFER_SDL /* !!! FIXME: Move to abstraction layer. --rcg. */
extern char *(__stdcall *pwglGetExtensionsStringARB)(HDC hdc);
extern BOOL  (__stdcall *pwglChoosePixelFormatARB)(HDC hdc, const int *piAttribIList, const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
extern BOOL  (__stdcall *pwglGetPixelFormatAttribivARB)(HDC hdc, int iPixelFormat, int iLayerPlane, UINT nAttributes, int *piAttributes, int *piValues);
#endif

// t-buffer support
extern void  (__stdcall *pglTBufferMask3DFX)(GLuint mask);

// GL_NV_vertex_array_range & GL_NV_fence
#if !SE1_PREFER_SDL /* !!! FIXME: Move to abstraction layer. --rcg. */
extern void *(__stdcall *pwglAllocateMemoryNV)(GLint size, GLfloat readfreq, GLfloat writefreq, GLfloat priority);
extern void  (__stdcall *pwglFreeMemoryNV)(void *pointer);
#endif

extern void  (__stdcall *pglVertexArrayRangeNV)(GLsizei length, void *pointer);
extern void  (__stdcall *pglFlushVertexArrayRangeNV)(void);

extern GLboolean (__stdcall *pglTestFenceNV)(GLuint fence);
extern GLboolean (__stdcall *pglIsFenceNV)(GLuint fence);
extern void  (__stdcall *pglGenFencesNV)(GLsizei n, GLuint *fences);
extern void  (__stdcall *pglDeleteFencesNV)(GLsizei n, const GLuint *fences);
extern void  (__stdcall *pglSetFenceNV)(GLuint fence, GLenum condition);
extern void  (__stdcall *pglFinishFenceNV)(GLuint fence);
extern void  (__stdcall *pglGetFenceivNV)(GLuint fence, GLenum pname, GLint *params);

// ATI GL_ATI[X]_pn_triangles
extern void  (__stdcall *pglPNTrianglesiATI)( GLenum pname, GLint param);
extern void  (__stdcall *pglPNTrianglesfATI)( GLenum pname, GLfloat param);


// additional tools -----------------------------------------------------


// set color from croteam format
inline void glCOLOR( COLOR col)
{
  col = ByteSwap32(col);
  pglColor4ubv((GLubyte*)&col);
}

extern BOOL glbUsingVARs;   // vertex_array_range

// common textures
extern GLuint _uiFillTextureNo;    // binding for flat fill emulator texture
extern GLuint _uiFogTextureNo;     // binding for fog texture
extern GLuint _uiHazeTextureNo;    // binding for haze texture
extern GLuint _uiPatternTextureNo; // binding for pattern texture

// [Cecil] NOTE: This works differently from glActiveTexture() from OpenGL 2.0+ and hence have been prefixed with "SE1_"
inline void SE1_glActiveTexture(INDEX texunit)
{
  ASSERT( texunit>=0 && texunit<4);
  ASSERT( pglActiveTextureARB!=NULL);
  ASSERT( pglClientActiveTextureARB!=NULL);
  pglActiveTextureARB(      GLenum(GL_TEXTURE0_ARB+texunit));
  pglClientActiveTextureARB(GLenum(GL_TEXTURE0_ARB+texunit));
}

#endif  /* include-once check. */

