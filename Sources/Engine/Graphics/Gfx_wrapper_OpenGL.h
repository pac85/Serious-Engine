/* Copyright (c) 2023 Dreamy Cecil
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

#ifndef SE_INCL_GFX_OGL_H
#define SE_INCL_GFX_OGL_H

#include <Engine/Graphics/GfxInterface.h>

class IGfxOpenGL : public IGfxInterface
{
  public:
    // Unique graphical API type
    virtual GfxAPIType GetType(void) const {
      return GAT_OGL;
    };

  // Graphical API methods
  public:

    // Enable operations
    virtual void EnableDepthWrite(void);
    virtual void EnableDepthBias(void);
    virtual void EnableDepthTest(void);
    virtual void EnableAlphaTest(void);
    virtual void EnableBlend(void);
    virtual void EnableDither(void);
    virtual void EnableTexture(void);
    virtual void EnableClipping(void);
    virtual void EnableClipPlane(void);

    // Disable operations
    virtual void DisableDepthWrite(void);
    virtual void DisableDepthBias(void);
    virtual void DisableDepthTest(void);
    virtual void DisableAlphaTest(void);
    virtual void DisableBlend(void);
    virtual void DisableDither(void);
    virtual void DisableTexture(void);
    virtual void DisableClipping(void);
    virtual void DisableClipPlane(void);

    // Set blending operations
    virtual void BlendFunc(GfxBlend eSrc, GfxBlend eDst);

    // Set depth buffer compare mode
    virtual void DepthFunc(GfxComp eFunc);

    // Set depth buffer range
    virtual void DepthRange(FLOAT fMin, FLOAT fMax);

    // Color mask control (use CT_RMASK, CT_GMASK, CT_BMASK, CT_AMASK to enable specific channels)
    virtual void SetColorMask(ULONG ulColorMask);

  // Projections
  public:

    // Set face culling
    virtual void CullFace(GfxFace eFace);
    virtual void FrontFace(GfxFace eFace);

    // Set custom clip plane (if NULL, disable it)
    virtual void ClipPlane(const DOUBLE *pdPlane);

    // Set orthographic matrix
    virtual void SetOrtho(const FLOAT fLeft, const FLOAT fRight, const FLOAT fTop,  const FLOAT fBottom, const FLOAT fNear, const FLOAT fFar, const BOOL bSubPixelAdjust);

    // Set frustrum matrix
    virtual void SetFrustum(const FLOAT fLeft, const FLOAT fRight, const FLOAT fTop,  const FLOAT fBottom, const FLOAT fNear, const FLOAT fFar);

    // Set view matrix
    virtual void SetViewMatrix(const FLOAT *pfMatrix);

    // Set texture matrix
    virtual void SetTextureMatrix(const FLOAT *pfMatrix);

    // Polygon mode (point, line or fill)
    virtual void PolygonMode(GfxPolyMode ePolyMode);

  // Textures
  public:

    // Set texture wrapping mode
    virtual void SetTextureWrapping(enum GfxWrap eWrapU, enum GfxWrap eWrapV);

    // Set texture modulation mode (1X or 2X)
    virtual void SetTextureModulation(INDEX iScale);

    // Generate texture for API
    virtual void GenerateTexture(ULONG &ulTexObject);

    // Unbind texture from API
    virtual void DeleteTexture(ULONG &ulTexObject);

  // Vertex arrays
  public:

    // Prepare vertex array for API
    virtual void SetVertexArray(void *pvtx, INDEX ctVtx);

    // Prepare normal array for API
    virtual void SetNormalArray(GFXNormal *pnor);

    // Prepare UV array for API
    virtual void SetTexCoordArray(GFXTexCoord *ptex, BOOL b4); // b4 = projective mapping (4 FLOATs)

    // Prepare color array for API
    virtual void SetColorArray(GFXColor *pcol);

    // Draw prepared arrays
    virtual void DrawElements(INDEX ctElem, INDEX *pidx);

    // Set constant color for subsequent rendering (until first SetColorArray() call!)
    virtual void SetConstantColor(COLOR col);

    // Color array usage control
    virtual void EnableColorArray(void);
    virtual void DisableColorArray(void);

  // Miscellaneous
  public:

    // Force finish of rendering queue
    virtual void Finish(void);

    // Compiled vertex array control
    virtual void LockArrays(void);

    // Toggle truform
    virtual void EnableTruform(void);
    virtual void DisableTruform(void);
};

#endif // include-once check
