#ifndef __GRAPHICSENGINE_H__
#define __GRAPHICSENGINE_H__

#pragma once

#include "GFX.h"
#include "GeometryBuffer.h"
#include "Font.h"
#include "Shader.h"

// ************************************************************************************************************************ //
// **
// ** state changes tracker
// **
// **
// **
// ************************************************************************************************************************ //

class CStateChangesTracker
{
  struct SState
  {
    DWORD type;// state ID
    DWORD dwCurrValue;// current setup value
    DWORD dwNeedValue;// value to setup
  };

  //
  std::unordered_map<DWORD, SState> allstates;
  std::list<SState *> changedstates;

public:
  // iterating
  using iterator = std::list<SState *>::iterator;
  using const_iterator = std::list<SState *>::const_iterator;
  iterator begin() { return changedstates.begin(); }
  iterator end() { return changedstates.end(); }
  const_iterator begin() const { return changedstates.begin(); }
  const_iterator end() const { return changedstates.end(); }
  void clear() { changedstates.clear(); }
  //
  void SetState(DWORD dwState, DWORD dwValue)
  {
    SState &state = allstates[dwState];
    if ((state.dwCurrValue != dwValue) && (state.dwNeedValue != dwValue))
    {
      state.type = dwState;
      state.dwNeedValue = dwValue;
      changedstates.push_back(&state);
    }
  }

  void ClearStates()
  {
    for (std::unordered_map<DWORD, SState>::iterator it = allstates.begin(); it != allstates.end(); ++it) it->second.dwNeedValue = it->second.dwCurrValue = -1;
    changedstates.clear();
  }
};

// ************************************************************************************************************************ //
// **
// ** main graphics engine class
// **
// **
// **
// ************************************************************************************************************************ //

class CGraphicsEngine : public IGFX
{
  OBJECT_NORMAL_METHODS(CGraphicsEngine);
  DECLARE_SERIALIZE;
  //
  HWND hWindow;// window handle, this engine attached to
  NWin32Helper::com_ptr<IDirect3D9> pD3D;// main Direct3D object
  NWin32Helper::com_ptr<IDirect3DDevice9> pD3DDevice;// current D3DDevice
  NWin32Helper::com_ptr<IDirect3DSurface9> pScreenColor;// frame buffer
  NWin32Helper::com_ptr<IDirect3DSurface9> pScreenDepth;// depth buffer
  CPtr<IGFXRTexture> pCurrRT;// current texture-as-render-target
  SAdapterDesc adapter;// selected adapter description
  D3DDISPLAYMODE displaymode;// current display mode
  D3DDISPLAYMODE desktopmode;// windows desktop mode
  D3DPRESENT_PARAMETERS pp;// presentation parameters
  RECT rcScreen;// screen placement
  int nStencilBPP;// current stencil BPP
  int nDepthBPP;// depth buffer BPP
  // gamma correction values
  float fBrightness;
  float fContrast;
  float fGamma;
  // viewport and matrices
  D3DVIEWPORT9 currviewport;
  std::list<D3DVIEWPORT9> viewports;
  SHMatrix matView, matBillboard;// viewport matrix (WCS => VCS)
  SHMatrix matInvView;// inverse view matrix
  SHMatrix matProjection;// projection matrix (VCS => PCS)
  SHMatrix matViewport;// viewport matrix (NDCS => SCS)
  SHMatrix matPick;// matrix for pick operations
  bool bDirectTransform;// are we in direct transform mode?
  SHMatrix matViewDirectStored;// view matrix, stored before direct transform
  SHMatrix matInvViewDirectStored;// inverse view matrix, stored before direct transform
  SHMatrix matBillboardDirectStored;// billboarding matrix, stored before direct transform
  // temporal storage for solid blocks
  CPtr2<CStaticVB> pSVB;// solid vertex buffer with static allocator
  CPtr2<CStaticIB> pSIB;// solid index buffer with static allocator
  // temp buffers
  std::unordered_map<DWORD, CPtr2<CTempVB>> tempVBs;
  CPtr2<CTempVB> pTVB;
  std::unordered_map<DWORD, CPtr2<CTempIB>> tempIBs;
  CPtr2<CTempIB> pTIB;
  bool bUseOptimizedBuffers;
  // dynamic buffers
  using CDynamicVBMap = std::unordered_multimap<DWORD, CPtr2<CDynamicVB>>;
  using CDynamicIBMap = std::unordered_multimap<DWORD, CPtr2<CDynamicIB>>;
  CDynamicVBMap dynVBs;
  CDynamicIBMap dynIBs;
  // last formats for flushing
  DWORD dwLastTempBufferFormat;
  DWORD dwLastVertexShader;
  // RSes and TSSes tracker
  CStateChangesTracker sctRS, sctTSS[8];
  // textures tracker
  std::vector<IGFXBaseTexture *> usedtextures;
  // CRAP{ for shaders testing
  using CShadersMap = std::unordered_map<int, CShader>;
  CShadersMap shaders;
  // CRAP}
  // fonts
  CPtr<CFont> pCurrentFont;
  // frame number
  int nCurrFrameNumber;
  // statistics
  int nNumPassedVertices;
  int nNumPassedPrimitives;
  DWORD dwLastFrameTime;
  //
  void ClearTempData();
  void FreeVideoMemory(int nUsage, int nAmount, bool bClearTempData);
  // CRAP{ for shaders testing
  void SetupShaders();
  // CRAP}
  //
  bool FindDepthStencilFormat(int nBPP, int nStencil);
  bool FillPresentationParams(int nWidth, int nHeight, int nBPP, int nStencilBPP, EGFXFullscreen eFullscreen, int nFreq);
  bool ResetDevice();
  void DestroyAllObjects();
  void ReCreateAllObjects();
  bool SetViewTransform(const CVec3 &ptX, const CVec3 &ptY, const CVec3 &ptZ, const CVec3 &ptO);
  void SetRenderState(D3DRENDERSTATETYPE state, int nValue);
  void SetTextureStageState(DWORD stage, int type, int value);
  void ApplyRenderStates();
  void ApplyTextureStageStates();

  void ApplyStates()
  {
    ApplyRenderStates();
    ApplyTextureStageStates();
  }

  void ClearStates();
  //
  bool IsFullscreen() const { return pp.Windowed == 0; }
  //
  bool SetupViewport(const D3DVIEWPORT9 &viewport);
  bool SetupLight(int nIndex, const D3DLIGHT9 &light);
  //
  void UpdatePickMatrix();
  //
  HRESULT RenderRange(CVertices *pVertices, CIndices *pIndices);
  HRESULT RenderRange(IDirect3DVertexBuffer9 *pVertices, int nFirstVertex, int nNumVertices, int nVertexSize,
                      IDirect3DIndexBuffer9 *pIndices, int nFirstIndex,
                      int nNumPrimitives, D3DPRIMITIVETYPE d3dptPrimitiveType);
  //
  void ForceFlushTempBuffers();
  //
  void SetVertexShader(DWORD dwFVF);
  // create empty geometry (vertex/index) buffer for 'nNumElements' elements with required format.
  // last 3 parameters are fake and required only for correct templpate instantiation due to MSVC bug
  struct SVBCreator
  {
    static const char *GetName() { return "vertex"; }
    static int GetElementSize(DWORD dwFormat) { return GetVertexSize(dwFormat); }
    static HRESULT CreateBuffer(IDirect3DDevice9 *pD3DDevice, int nSizeInBytes, DWORD dwUsage, DWORD dwFormat, D3DPOOL pool, IDirect3DVertexBuffer9 **ppD3DBuffer) { return pD3DDevice->CreateVertexBuffer(nSizeInBytes, dwUsage, dwFormat, pool, ppD3DBuffer, nullptr); }
  };

  struct SIBCreator
  {
    static const char *GetName() { return "index"; }
    static int GetElementSize(DWORD dwFormat) { return GetIndexSize(dwFormat); }
    static HRESULT CreateBuffer(IDirect3DDevice9 *pD3DDevice, int nSizeInBytes, DWORD dwUsage, DWORD dwFormat, D3DPOOL pool, IDirect3DIndexBuffer9 **ppD3DBuffer) { return pD3DDevice->CreateIndexBuffer(nSizeInBytes, dwUsage, D3DFORMAT(dwFormat), pool, ppD3DBuffer, nullptr); }
  };

  template<class TBuffer, class TD3DBuffer, class TCreator>
  TBuffer *CreateGeometryBuffer(int nNumElements, DWORD dwFormat, EGFXDynamic eDynamic, TBuffer *, TD3DBuffer *, TCreator *)
  {
    int nElementSize = TCreator::GetElementSize(dwFormat);
    nNumElements = TBuffer::GetOptimalSize(nNumElements, nElementSize);
    DWORD dwUsage = eDynamic == GFXD_DYNAMIC ? D3DUSAGE_DYNAMIC : 0;// dynamic
    dwUsage |= adapter.dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING ? D3DUSAGE_SOFTWAREPROCESSING : 0;// SW T&L
    D3DPOOL pool = adapter.dwBehavior & D3DCREATE_SOFTWARE_VERTEXPROCESSING ? D3DPOOL_SYSTEMMEM : D3DPOOL_DEFAULT;
    dwUsage |= D3DUSAGE_WRITEONLY;// write only!!!
    //
    TD3DBuffer *pD3DBuffer = nullptr;
    HRESULT dxrval = TCreator::CreateBuffer(pD3DDevice, nElementSize * nNumElements, dwUsage, dwFormat, pool, &pD3DBuffer);
    if (FAILED(dxrval))
    {
      FreeVideoMemory(nCurrFrameNumber - 5, nElementSize * nNumElements, true);
      dxrval = TCreator::CreateBuffer(pD3DDevice, nElementSize * nNumElements, dwUsage, dwFormat, pool, &pD3DBuffer);
      if (FAILED(dxrval))
      {
        FreeVideoMemory(nCurrFrameNumber, nElementSize * nNumElements, true);
        dxrval = TCreator::CreateBuffer(pD3DDevice, nElementSize * nNumElements, dwUsage, dwFormat, pool, &pD3DBuffer);
        if (FAILED(dxrval))
        {
          FreeVideoMemory(nCurrFrameNumber + 1, nElementSize * nNumElements, true);
          dxrval = TCreator::CreateBuffer(pD3DDevice, nElementSize * nNumElements, dwUsage, dwFormat, pool, &pD3DBuffer);
        }
      }
      NI_ASSERTHR_TF(dxrval, NStr::Format("Can't create %s buffer for %d elements with %d format", TCreator::GetName(), nNumElements, dwFormat), return 0);
    }
    //
    TBuffer *pBuffer = new TBuffer(pD3DBuffer, nElementSize, dwFormat, nNumElements, eDynamic == GFXD_DYNAMIC);
    pD3DBuffer->Release();
    //
    return pBuffer;
  }

  // dynamic IBs and VBs functions
  template<class TBuffer, class TD3DBuffer, class TCreator>
  TBuffer *GetDynamicBuffer(int nNumElements, DWORD dwFormat, std::unordered_multimap<DWORD, CPtr2<TBuffer>> &buffers, TD3DBuffer *, TCreator *)
  {
    using CDynBuffersMap = std::unordered_multimap<DWORD, CPtr2<TBuffer>>;
    using CDynBuffersRange = std::pair<CDynBuffersMap::iterator, CDynBuffersMap::iterator>;
    CDynBuffersRange range = buffers.equal_range(dwFormat);
    for (CDynBuffersMap::iterator it = range.first; it != range.second; ++it) { if (it->second->HasSolidBlock(nNumElements)) return it->second.GetPtr(); }
    // don't have a required block. 
    // int nElementSize = TCreator::GetElementSize( dwFormat );
    // nNumElements = Max( GetNextPow2( nNumElements ), 65535 / nElementSize );
    CPtr2<TBuffer> pBuffer = CreateGeometryBuffer(nNumElements, dwFormat, GFXD_DYNAMIC, static_cast<TBuffer *>(nullptr), static_cast<TD3DBuffer *>(nullptr), static_cast<TCreator *>(nullptr));
    buffers.insert(CDynBuffersMap::value_type(dwFormat, pBuffer));
    return pBuffer;
  }

public:
  CGraphicsEngine()
    : fBrightness(0), fContrast(0), fGamma(0), bDirectTransform(false), bUseOptimizedBuffers(false), dwLastTempBufferFormat(0), dwLastVertexShader(0) {}

  CGraphicsEngine(const SAdapterDesc *pAdapter)
    : adapter(*pAdapter), fBrightness(0), fContrast(0), fGamma(0), bDirectTransform(false), bUseOptimizedBuffers(false), dwLastTempBufferFormat(0), dwLastVertexShader(0) {}

  virtual ~CGraphicsEngine() { CGraphicsEngine::Done(); }

  // initialization and setup
  bool Init(const char *pszAdapterName, HWND hWnd) override;
  bool Done() override;
  void Clear() override;
  bool SetMode(int nSizeX, int nSizeY, int nBpp, int nStencilBPP, EGFXFullscreen eFullscreen, int nFreq) override;
  EGFXVideoCard GetVideoCard() override;

  // move GFX screen to the new position
  void MoveTo(int nX, int nY) override;

  // screen and adapter info
  RECT GetScreenRect() const override { return rcScreen; }
  int GetScreenBPP() const override { return GetBPP(displaymode.Format); }
  const char * GetAdapterName() const override { return adapter.szDescription.c_str(); }
  const struct SGFXDisplayMode * GetDisplayModes() const override;

  // T&L setup functions
  // viewport management
  void PushViewport() override;
  bool PopViewport() override;
  bool ChangeViewport(int nX, int nY, int nWidth, int nHeight, float fMinZ, float fMaxZ) override;
  bool ChangeViewport(int nWidth, int nHeight) override;
  // transforms: view, world, projection, texture
  bool SetWorldTransforms(int nStartIndex, const SHMatrix *pMatrices, int nNumMatrices) override;
  bool SetViewTransform(const SHMatrix &matrix) override;
  bool SetProjectionTransform(const SHMatrix &matrix) override;
  bool SetTextureTransform(int nIndex, const SHMatrix &matrix) override;
  bool SetupDirectTransform() override;
  bool RestoreTransform() override;
  //
  const SHMatrix & GetViewMatrix() const override;
  const SHMatrix & GetBillboardMatrix() const override;
  const SHMatrix & GetInverseViewMatrix() const override;
  const SHMatrix & GetProjectionMatrix() const override;
  const SHMatrix & GetViewportMatrix() const override;
  void GetViewVolume(SPlane *pPlanes) const override;
  void GetViewVolumeCrosses(const CVec2 &vPoint, CVec3 *pvNear, CVec3 *pvFar) override;
  // lighting properties
  void SetLight(int nIndex, const SGFXLightDirectional &light) override;
  void SetLight(int nIndex, const SGFXLightPoint &light) override;
  void SetLight(int nIndex, const SGFXLightSpot &light) override;
  void EnableLight(int nIndex, bool bEnable) override;
  void SetMaterial(const SGFXMaterial &material) override;

  // texture setup
  bool SetTexture(int nStage, IGFXBaseTexture *pTexture) override;

  // state setup
  bool SetWireframe(bool bWireframe) override;
  bool SetCullMode(EGFXCull cull) override;
  bool SetDepthBufferMode(EGFXDepthBuffer depth, EGFXCmpFunction cmp) override;
  bool EnableLighting(bool bLighting) override;
  bool EnableSpecular(bool bEnable) override;

  // font setup
  bool SetFont(IGFXFont *pFont) override;

  // screen management
  bool IsActive() override;
  bool BeginScene() override;
  bool EndScene() override;
  bool Clear(int nNumRects, RECT *pRects, DWORD dwFlags, DWORD dwColor, float fDepth, DWORD dwStencil) override;
  bool Flip() override;
  bool SetRenderTarget(IGFXRTexture *pRT) override;

  // geometry
  void SetOptimizedBuffers(bool bEnable) override;
  // vertices/indices
  IGFXVertices * CreateVertices(int nNumElements, DWORD dwFormat, EGFXPrimitiveType type, EGFXDynamic eDynamic, IGFXVertices *pVertices = nullptr) override;
  IGFXIndices * CreateIndices(int nNumElements, DWORD dwFormat, EGFXPrimitiveType type, EGFXDynamic eDynamic, IGFXIndices *pIndices = nullptr) override;
  // solid blocks
  bool BeginSolidVertexBlock(int nNumElements, DWORD dwFormat, EGFXDynamic eDynamic) override;
  bool EndSolidVertexBlock() override;
  bool BeginSolidIndexBlock(int nNumElements, DWORD dwFormat, EGFXDynamic eDynamic) override;
  bool EndSolidIndexBlock() override;
  // temp geometry
  void * GetTempVertices(int nNumElements, DWORD dwFormat, EGFXPrimitiveType type) override;
  void * GetTempIndices(int nNumElements, DWORD dwFormat, EGFXPrimitiveType type) override;

  // texture
  IGFXTexture * CreateTexture(int nSizeX, int nSizeY, int nNumMipLevels, EGFXPixelFormat format, EGFXDynamic eDynamic, IGFXTexture *pTexture = nullptr) override;
  IGFXRTexture * CreateRTexture(int nSizeX, int nSizeY) override;
  bool UpdateTexture(IGFXTexture *pSrc, IGFXTexture *pDst, bool bAsync = true) override;

  // rendering
  bool Draw(IGFXVertices *pVertices, IGFXIndices *pIndices) override;
  bool DrawTemp() override;
  bool DrawMesh(IGFXMesh *pMesh, const SHMatrix *matrices, int nNumMatrices) override;
  bool DrawStringA(const char *pszString, int nX, int nY, DWORD dwColor) override;
  bool DrawString(const wchar_t *pszString, int nX, int nY, DWORD dwColor) override;
  bool DrawText(IGFXText *pText, const RECT &rect, int nY, DWORD dwFlags = FNT_FORMAT_LEFT) override;
  bool DrawRects(const SGFXRect2 *pRects, int nNumRects, bool bSolid) override;

  // gamma ramp
  bool SetGammaRamp(const SGFXGammaRamp &ramp, bool bCalibrate) override;
  bool GetGammaRamp(const SGFXGammaRamp *pRamp) override;
  void SetGammaCorrectionValues(float fBrightness, float fContrast, float fGamma) override;
  void GetGammaCorrectionValues(float *pfBrightness, float *pfContrast, float *pfGamma) override;

  // screenshot
  bool TakeScreenShot(interface IImage *pImage) override;

  // statistics
  int GetNumPassedVertices() const override { return nNumPassedVertices; }
  int GetNumPassedPrimitives() const override { return nNumPassedPrimitives; }

  // temporal function before shaders
  bool SetShadingEffect(int nEffect) override;
};

#endif // __GRAPHICSENGINE_H__