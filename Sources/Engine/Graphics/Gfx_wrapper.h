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

#ifndef SE_INCL_GFX_WRAPPER_H
#define SE_INCL_GFX_WRAPPER_H

#ifdef PRAGMA_ONCE
  #pragma once
#endif

// [Cecil] Moved from GfxLibrary.h
enum GfxAPIType
{
  GAT_NONE = -1, // no gfx API (gfx functions are disabled)
  GAT_OGL  =  0, // OpenGL
  GAT_D3D  =  1, // Direct3D

  GAT_MAX, // [Cecil] Amount of graphics APIs

  GAT_CURRENT = 9, // current API
};

enum GfxBlend
{
  GFX_ONE           = 21,
  GFX_ZERO          = 22,
  GFX_SRC_COLOR     = 23,
  GFX_INV_SRC_COLOR = 24,
  GFX_DST_COLOR     = 25,
  GFX_INV_DST_COLOR = 26,
  GFX_SRC_ALPHA     = 27,
  GFX_INV_SRC_ALPHA = 28,
};

enum GfxComp
{
  GFX_NEVER         = 41,
  GFX_LESS          = 42,
  GFX_LESS_EQUAL    = 43,
  GFX_EQUAL         = 44,
  GFX_NOT_EQUAL     = 45,
  GFX_GREATER_EQUAL = 46,
  GFX_GREATER       = 47,
  GFX_ALWAYS        = 48,
};
  
enum GfxFace
{
  GFX_NONE  = 61,
  GFX_FRONT = 62,
  GFX_BACK  = 63,
  GFX_CW    = 64,
  GFX_CCW   = 65,
};

enum GfxMatrixType
{
  GFX_VIEW       = 71,
  GFX_PROJECTION = 72,
};

enum GfxWrap
{
  GFX_REPEAT = 81,
  GFX_CLAMP  = 82,
};

enum GfxPolyMode
{
  GFX_FILL  = 91,
  GFX_LINE  = 92,
  GFX_POINT = 93,
};

// TEXTURES

// texture settings (holds current states of texture quality, size and such)
struct TextureSettings {
public:
  //quailties
  INDEX ts_iNormQualityO;    
  INDEX ts_iNormQualityA;
  INDEX ts_iAnimQualityO;
  INDEX ts_iAnimQualityA;
  // sizes/forcing
  PIX ts_pixNormSize;
  PIX ts_pixAnimSize;
  // texture formats (set by OGL or D3D)
  ULONG ts_tfRGB8, ts_tfRGBA8;               // true color
  ULONG ts_tfRGB5, ts_tfRGBA4, ts_tfRGB5A1;  // high color
  ULONG ts_tfLA8,  ts_tfL8;                  // grayscale
  ULONG ts_tfCRGB, ts_tfCRGBA;               // compressed formats
  // maximum texel-byte ratio for largest texture size
  INDEX ts_iMaxBytesPerTexel;
};
// singleton object for texture settings
extern struct TextureSettings TS;
// routine for updating texture settings from console variable
extern void UpdateTextureSettings(void);


// texture parameters for texture state changes
class CTexParams {
public:
  INDEX tp_iFilter;            // OpenGL texture mapping mode
  INDEX tp_iAnisotropy;        // texture degree of anisotropy (>=1.0f; 1.0=isotropic, default)
  BOOL  tp_bSingleMipmap;      // texture has only one mipmap
  GfxWrap tp_eWrapU, tp_eWrapV;  // wrapping states
  inline CTexParams(void) { Clear(); tp_bSingleMipmap = FALSE; };
  inline void Clear(void) { tp_iFilter = 00; tp_iAnisotropy = 0; tp_eWrapU = tp_eWrapV = (GfxWrap)NONE; };
  inline BOOL IsEqual( CTexParams tp) { return tp_iFilter==tp.tp_iFilter && tp_iAnisotropy==tp.tp_iAnisotropy && 
                                               tp_eWrapU==tp.tp_eWrapU && tp_eWrapV==tp.tp_eWrapV; };
};

// MISC

// check GFX errors only in debug builds
#ifndef NDEBUG
  extern void OGL_CheckError(void);
  #define OGL_CHECKERROR OGL_CheckError();

#if SE1_DIRECT3D
  extern void D3D_CheckError(HRESULT hr);
  #define D3D_CHECKERROR(hr) D3D_CheckError(hr);
#endif

#else
  #define OGL_CHECKERROR     (void)(0);
  #define D3D_CHECKERROR(hr) (void)(0);
#endif

// macro for releasing D3D objects
#define D3DRELEASE(object,check) \
{ \
  INDEX ref; \
  do { \
    ref = (object)->Release(); \
    if(check) ASSERT(ref==0); \
  } while(ref>0);  \
  object = NONE; \
}

#endif // include-once check
