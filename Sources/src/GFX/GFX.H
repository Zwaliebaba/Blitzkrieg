#pragma once

#include "GFXTypes.h"

#undef DrawText

enum
{
  GFX_BASE_VALUE = 0x10030000,
  GFX_GFX = GFX_BASE_VALUE + 1,
  GFX_TEXTURE_MANAGER = GFX_BASE_VALUE + 2,
  GFX_MESH_MANAGER = GFX_BASE_VALUE + 3,
  GFX_FONT_MANAGER = GFX_BASE_VALUE + 4,
  GFX_TEXTURE = GFX_BASE_VALUE + 5,
  GFX_CUBE_TEXTURE = GFX_BASE_VALUE + 6,
  GFX_VOLUME_TEXTURE = GFX_BASE_VALUE + 7,
  GFX_SURFACE = GFX_BASE_VALUE + 8,
  GFX_VERTICES = GFX_BASE_VALUE + 9,
  GFX_INDICES = GFX_BASE_VALUE + 10,
  GFX_MESH = GFX_BASE_VALUE + 11,
  GFX_FONT = GFX_BASE_VALUE + 12,
  GFX_TEXT = GFX_BASE_VALUE + 13,
  GFX_RT_TEXTURE = GFX_BASE_VALUE + 14
};

// ************************************************************************************************************************ //
// **
// ** geometry section
// **
// **
// **
// ************************************************************************************************************************ //

interface IGFXVertices : ISharedResource
{
  // direct data access
  virtual void * Lock() = 0;
  virtual void Unlock() = 0;
};

interface IGFXIndices : ISharedResource
{
  // direct data access
  virtual void * Lock() = 0;
  virtual void Unlock() = 0;
  // set number of vertices, which is addressed by this index set
  virtual void SetNumUsedVertices(int nNumUsedVertices) = 0;
};

interface IGFXMesh : ISharedResource
{
  virtual const SGFXBoundSphere & GetBS() = 0;
  virtual const SGFXAABB & GetAABB() = 0;
  virtual bool IsHit(const CVec2 &vPos, const SHMatrix *matrices) = 0;
};

interface IMeshManager : ISharedManager
{
  // type ID
  enum { tidTypeID = GFX_MESH_MANAGER };

  virtual IGFXMesh * GetMesh(const char *pszName) = 0;
};

// ************************************************************************************************************************ //
// **
// ** surfaces and textures section
// **
// **
// **
// ************************************************************************************************************************ //

interface IGFXBaseTexture : ISharedResource {};

interface IGFXTexture : IGFXBaseTexture
{
  // direct data access through lock
  virtual bool Lock(int nLevel, SSurfaceLockInfo *pLockInfo) = 0;
  virtual bool Unlock(int nLevel) = 0;
  // add dirty rect for further update
  virtual bool AddDirtyRect(const RECT *pRect) = 0;
  // size access
  virtual int GetSizeX(int nLevel) const = 0;
  virtual int GetSizeY(int nLevel) const = 0;
  virtual EGFXPixelFormat GetFormat() const = 0;
};

interface IGFXRTexture : IGFXBaseTexture
{
  virtual int GetSizeX(int nLevel) const = 0;
  virtual int GetSizeY(int nLevel) const = 0;
  virtual EGFXPixelFormat GetFormat() const = 0;
};

interface IGFXSurface : IRefCount
{
  // direct data access through lock
  virtual bool Lock(SSurfaceLockInfo *pLockInfo) = 0;
  virtual bool Unlock() = 0;
  // size access
  virtual int GetSizeX() const = 0;
  virtual int GetSizeY() const = 0;
  virtual EGFXPixelFormat GetFormat() const = 0;
};

interface ITextureManager : ISharedManager
{
  // type ID
  enum { tidTypeID = GFX_TEXTURE_MANAGER };

  //
  enum ETextureQuality
  {
    TEXTURE_QUALITY_COMPRESSED = 1,
    TEXTURE_QUALITY_LOW = 2,
    TEXTURE_QUALITY_HIGH = 3,
  };

  //
  virtual IGFXTexture * GetTexture(const char *pszName) = 0;
  virtual const char * GetTextureName(IGFXTexture *pTexture) = 0;
  //
  virtual void SetQuality(ETextureQuality eQuality) = 0;
};

// ************************************************************************************************************************ //
// **
// ** font and text classes
// **
// **
// **
// ************************************************************************************************************************ //

interface IGFXFont : ISharedResource
{
  virtual int GetHeight() const = 0;
  virtual int GetLineSpace() const = 0;
  virtual int GetAscent() const = 0;
  virtual int GetDescent() const = 0;
  //
  virtual const struct SFontFormat & GetFormat() const = 0;
  virtual int GetTextWidth(const char *pszString, int nCounter = 2000000000) const = 0;
  virtual int GetTextWidth(const wchar_t *pszString, int nCounter = 2000000000) const = 0;
};

interface IGFXText : IRefCount
{
  virtual void SetFont(IGFXFont *pFont) = 0;
  virtual void SetText(interface IText *pText) = 0;
  virtual interface IText * GetText() = 0;
  virtual void SetWidth(int nWidth) = 0;
  virtual void SetColor(DWORD color) = 0;
  virtual void EnableRedLine(bool bEnable) = 0;
  virtual void SetRedLine(int nRedLineIndention) = 0;
  //
  virtual void SetChanged() = 0;
  // stats
  virtual int GetNumLines() const = 0;
  virtual int GetLineSpace() const = 0;
  virtual int GetWidth(int nNumCharacters = -1) const = 0;
};

interface IFontManager : ISharedManager
{
  // type ID
  enum { tidTypeID = GFX_FONT_MANAGER };

  //
  virtual IGFXFont * GetFont(const char *pszName) = 0;
};

// ************************************************************************************************************************ //
// **
// ** main graphics engine
// **
// **
// **
// ************************************************************************************************************************ //

interface IGFX : IRefCount
{
  // type ID
  enum { tidTypeID = GFX_GFX };

  // initialization and setup
  virtual bool Init(const char *pszAdapterName, HWND hWnd) = 0;
  virtual bool Done() = 0;
  virtual void Clear() = 0;
  virtual bool SetMode(int nSizeX, int nSizeY, int nBpp, int nStencilBPP, EGFXFullscreen eFullscreen, int nFreq = 0) = 0;
  virtual EGFXVideoCard GetVideoCard() = 0;

  // move GFX screen to the new position
  virtual void MoveTo(int nX, int nY) = 0;

  // screen and adapter info
  virtual RECT GetScreenRect() const = 0;
  virtual int GetScreenBPP() const = 0;
  virtual const char * GetAdapterName() const = 0;
  virtual const struct SGFXDisplayMode * GetDisplayModes() const = 0;

  // T&L setup functions
  // viewport management
  virtual void PushViewport() = 0;
  virtual bool PopViewport() = 0;
  virtual bool ChangeViewport(int nX, int nY, int nWidth, int nHeight, float fMinZ, float fMaxZ) = 0;
  virtual bool ChangeViewport(int nWidth, int nHeight) = 0;
  // transforms: view, world, projection, texture
  virtual bool SetWorldTransforms(int nStartIndex, const SHMatrix *pMatrices, int nNumMatrices) = 0;
  virtual bool SetViewTransform(const SHMatrix &matrix) = 0;
  virtual bool SetProjectionTransform(const SHMatrix &matrix) = 0;
  virtual bool SetTextureTransform(int nIndex, const SHMatrix &matrix) = 0;
  virtual bool SetupDirectTransform() = 0;
  virtual bool RestoreTransform() = 0;
  //
  virtual const SHMatrix & GetViewMatrix() const = 0;
  virtual const SHMatrix & GetBillboardMatrix() const = 0;
  virtual const SHMatrix & GetInverseViewMatrix() const = 0;
  virtual const SHMatrix & GetProjectionMatrix() const = 0;
  virtual const SHMatrix & GetViewportMatrix() const = 0;
  // retrieve view volume planes in world space
  virtual void GetViewVolume(SPlane *pPlanes) const = 0;
  // retrieve intersection points with far and near clip planes
  virtual void GetViewVolumeCrosses(const CVec2 &vPoint, CVec3 *pvNear, CVec3 *pvFar) = 0;
  // lighting properties
  virtual void SetLight(int nIndex, const SGFXLightDirectional &light) = 0;
  virtual void SetLight(int nIndex, const SGFXLightPoint &light) = 0;
  virtual void SetLight(int nIndex, const SGFXLightSpot &light) = 0;
  virtual void EnableLight(int nIndex, bool bEnable) = 0;
  virtual void SetMaterial(const SGFXMaterial &material) = 0;

  // texture setup
  virtual bool SetTexture(int nStage, IGFXBaseTexture *pTexture) = 0;

  // state setup
  virtual bool SetWireframe(bool bWireframe) = 0;
  virtual bool SetCullMode(EGFXCull cull) = 0;
  virtual bool SetDepthBufferMode(EGFXDepthBuffer depth, EGFXCmpFunction cmp = GFXCMP_DEFAULT) = 0;
  virtual bool EnableLighting(bool bLighting) = 0;
  virtual bool EnableSpecular(bool bEnable) = 0;

  // font setup
  virtual bool SetFont(IGFXFont *pFont) = 0;

  // screen management
  virtual bool IsActive() = 0;
  virtual bool BeginScene() = 0;
  virtual bool EndScene() = 0;
  virtual bool Clear(int nNumRects, RECT *pRects, DWORD dwFlags, DWORD dwColor = 0, float fDepth = 1.0f, DWORD dwStencil = 0) = 0;
  virtual bool Flip() = 0;
  virtual bool SetRenderTarget(IGFXRTexture *pRT) = 0;

  // geometry
  virtual void SetOptimizedBuffers(bool bEnable) = 0;
  // vertices/indices
  virtual IGFXVertices * CreateVertices(int nNumElements, DWORD dwFormat, EGFXPrimitiveType type, EGFXDynamic eDynamic, IGFXVertices *pVertices = nullptr) = 0;
  virtual IGFXIndices * CreateIndices(int nNumElements, DWORD dwFormat, EGFXPrimitiveType type, EGFXDynamic eDynamic, IGFXIndices *pIndices = nullptr) = 0;
  // solid blocks
  virtual bool BeginSolidVertexBlock(int nNumElements, DWORD dwFormat, EGFXDynamic eDynamic) = 0;
  virtual bool EndSolidVertexBlock() = 0;
  virtual bool BeginSolidIndexBlock(int nNumElements, DWORD dwFormat, EGFXDynamic eDynamic) = 0;
  virtual bool EndSolidIndexBlock() = 0;
  // temp geometry
  virtual void * GetTempVertices(int nNumElements, DWORD dwFormat, EGFXPrimitiveType type) = 0;
  virtual void * GetTempIndices(int nNumElements, DWORD dwFormat, EGFXPrimitiveType type) = 0;

  // texture
  virtual IGFXTexture * CreateTexture(int nSizeX, int nSizeY, int nNumMipLevels, EGFXPixelFormat format, EGFXDynamic eDynamic, IGFXTexture *pTexture = nullptr) = 0;
  virtual IGFXRTexture * CreateRTexture(int nSizeX, int nSizeY) = 0;
  virtual bool UpdateTexture(IGFXTexture *pSrc, IGFXTexture *pDst, bool bAsync = true) = 0;

  // rendering
  virtual bool Draw(IGFXVertices *pVertices, IGFXIndices *pIndices) = 0;
  virtual bool DrawTemp() = 0;
  virtual bool DrawMesh(IGFXMesh *pMesh, const SHMatrix *matrices, int nNumMatrices) = 0;
  virtual bool DrawStringA(const char *pszString, int nX, int nY, DWORD dwColor = 0xFFFFFFFF) = 0;
  virtual bool DrawString(const wchar_t *pszString, int nX, int nY, DWORD dwColor = 0xFFFFFFFF) = 0;
  virtual bool DrawText(IGFXText *pText, const RECT &rect, int nY, DWORD dwFlags = FNT_FORMAT_LEFT) = 0;
  virtual bool DrawRects(const SGFXRect2 *pRects, int nNumRects, bool bSolid = true) = 0;

  // gamma ramp
  virtual bool SetGammaRamp(const SGFXGammaRamp &ramp, bool bCalibrate) = 0;
  virtual bool GetGammaRamp(const SGFXGammaRamp *pRamp) = 0;
  virtual void SetGammaCorrectionValues(float fBrightness, float fContrast, float fGamma) = 0;
  virtual void GetGammaCorrectionValues(float *pfBrightness, float *pfContrast, float *pfGamma) = 0;

  // screenshot
  virtual bool TakeScreenShot(interface IImage *pImage) = 0;

  // statistics
  virtual int GetNumPassedVertices() const = 0;
  virtual int GetNumPassedPrimitives() const = 0;

  // temporal function before shaders
  virtual bool SetShadingEffect(int nEffect) = 0;
};
