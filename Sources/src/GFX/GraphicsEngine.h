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
  NWin32Helper::com_ptr<IDirect3D8> pD3D;// main Direct3D object
  NWin32Helper::com_ptr<IDirect3DDevice8> pD3DDevice;// current D3DDevice
  NWin32Helper::com_ptr<IDirect3DSurface8> pScreenColor;// frame buffer
  NWin32Helper::com_ptr<IDirect3DSurface8> pScreenDepth;// depth buffer
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
  D3DVIEWPORT8 currviewport;
  std::list<D3DVIEWPORT8> viewports;
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
  void SetTextureStageState(DWORD stage, D3DTEXTURESTAGESTATETYPE type, int value);
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
  bool SetupViewport(const D3DVIEWPORT8 &viewport);
  bool SetupLight(int nIndex, const D3DLIGHT8 &light);
  //
  void UpdatePickMatrix();
  //
  HRESULT RenderRange(CVertices *pVertices, CIndices *pIndices);
  HRESULT RenderRange(IDirect3DVertexBuffer8 *pVertices, int nFirstVertex, int nNumVertices, int nVertexSize,
                      IDirect3DIndexBuffer8 *pIndices, int nFirstIndex,
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
    static HRESULT CreateBuffer(IDirect3DDevice8 *pD3DDevice, int nSizeInBytes, DWORD dwUsage, DWORD dwFormat, D3DPOOL pool, IDirect3DVertexBuffer8 **ppD3DBuffer) { return pD3DDevice->CreateVertexBuffer(nSizeInBytes, dwUsage, dwFormat, pool, ppD3DBuffer); }
  };

  struct SIBCreator
  {
    static const char *GetName() { return "index"; }
    static int GetElementSize(DWORD dwFormat) { return GetIndexSize(dwFormat); }
    static HRESULT CreateBuffer(IDirect3DDevice8 *pD3DDevice, int nSizeInBytes, DWORD dwUsage, DWORD dwFormat, D3DPOOL pool, IDirect3DIndexBuffer8 **ppD3DBuffer) { return pD3DDevice->CreateIndexBuffer(nSizeInBytes, dwUsage, D3DFORMAT(dwFormat), pool, ppD3DBuffer); }
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
  bool STDCALL Init(const char *pszAdapterName, HWND hWnd) override;
  bool STDCALL Done() override;
  void Clear() override;
  bool STDCALL SetMode(int nSizeX, int nSizeY, int nBpp, int nStencilBPP, EGFXFullscreen eFullscreen, int nFreq) override;
  EGFXVideoCard STDCALL GetVideoCard() override;

  // move GFX screen to the new position
  void STDCALL MoveTo(int nX, int nY) override;

  // screen and adapter info
  RECT STDCALL GetScreenRect() const override { return rcScreen; }
  int STDCALL GetScreenBPP() const override { return GetBPP(displaymode.Format); }
  const char * STDCALL GetAdapterName() const override { return adapter.szDescription.c_str(); }
  const struct SGFXDisplayMode * STDCALL GetDisplayModes() const override;

  // T&L setup functions
  // viewport management
  void STDCALL PushViewport() override;
  bool STDCALL PopViewport() override;
  bool STDCALL ChangeViewport(int nX, int nY, int nWidth, int nHeight, float fMinZ, float fMaxZ) override;
  bool STDCALL ChangeViewport(int nWidth, int nHeight) override;
  // transforms: view, world, projection, texture
  bool STDCALL SetWorldTransforms(int nStartIndex, const SHMatrix *pMatrices, int nNumMatrices) override;
  bool STDCALL SetViewTransform(const SHMatrix &matrix) override;
  bool STDCALL SetProjectionTransform(const SHMatrix &matrix) override;
  bool STDCALL SetTextureTransform(int nIndex, const SHMatrix &matrix) override;
  bool STDCALL SetupDirectTransform() override;
  bool STDCALL RestoreTransform() override;
  //
  const SHMatrix & STDCALL GetViewMatrix() const override;
  const SHMatrix & STDCALL GetBillboardMatrix() const override;
  const SHMatrix & STDCALL GetInverseViewMatrix() const override;
  const SHMatrix & STDCALL GetProjectionMatrix() const override;
  const SHMatrix & STDCALL GetViewportMatrix() const override;
  void STDCALL GetViewVolume(SPlane *pPlanes) const override;
  void STDCALL GetViewVolumeCrosses(const CVec2 &vPoint, CVec3 *pvNear, CVec3 *pvFar) override;
  // lighting properties
  void STDCALL SetLight(int nIndex, const SGFXLightDirectional &light) override;
  void STDCALL SetLight(int nIndex, const SGFXLightPoint &light) override;
  void STDCALL SetLight(int nIndex, const SGFXLightSpot &light) override;
  void STDCALL EnableLight(int nIndex, bool bEnable) override;
  void STDCALL SetMaterial(const SGFXMaterial &material) override;

  // texture setup
  bool STDCALL SetTexture(int nStage, IGFXBaseTexture *pTexture) override;

  // state setup
  bool STDCALL SetWireframe(bool bWireframe) override;
  bool STDCALL SetCullMode(EGFXCull cull) override;
  bool STDCALL SetDepthBufferMode(EGFXDepthBuffer depth, EGFXCmpFunction cmp) override;
  bool STDCALL EnableLighting(bool bLighting) override;
  bool STDCALL EnableSpecular(bool bEnable) override;

  // font setup
  bool STDCALL SetFont(IGFXFont *pFont) override;

  // screen management
  bool STDCALL IsActive() override;
  bool STDCALL BeginScene() override;
  bool STDCALL EndScene() override;
  bool STDCALL Clear(int nNumRects, RECT *pRects, DWORD dwFlags, DWORD dwColor, float fDepth, DWORD dwStencil) override;
  bool STDCALL Flip() override;
  bool STDCALL SetRenderTarget(IGFXRTexture *pRT) override;

  // geometry
  void STDCALL SetOptimizedBuffers(bool bEnable) override;
  // vertices/indices
  IGFXVertices * STDCALL CreateVertices(int nNumElements, DWORD dwFormat, EGFXPrimitiveType type, EGFXDynamic eDynamic, IGFXVertices *pVertices = nullptr) override;
  IGFXIndices * STDCALL CreateIndices(int nNumElements, DWORD dwFormat, EGFXPrimitiveType type, EGFXDynamic eDynamic, IGFXIndices *pIndices = nullptr) override;
  // solid blocks
  bool STDCALL BeginSolidVertexBlock(int nNumElements, DWORD dwFormat, EGFXDynamic eDynamic) override;
  bool STDCALL EndSolidVertexBlock() override;
  bool STDCALL BeginSolidIndexBlock(int nNumElements, DWORD dwFormat, EGFXDynamic eDynamic) override;
  bool STDCALL EndSolidIndexBlock() override;
  // temp geometry
  void * STDCALL GetTempVertices(int nNumElements, DWORD dwFormat, EGFXPrimitiveType type) override;
  void * STDCALL GetTempIndices(int nNumElements, DWORD dwFormat, EGFXPrimitiveType type) override;

  // texture
  IGFXTexture * STDCALL CreateTexture(int nSizeX, int nSizeY, int nNumMipLevels, EGFXPixelFormat format, EGFXDynamic eDynamic, IGFXTexture *pTexture = nullptr) override;
  IGFXRTexture * STDCALL CreateRTexture(int nSizeX, int nSizeY) override;
  bool STDCALL UpdateTexture(IGFXTexture *pSrc, IGFXTexture *pDst, bool bAsync = true) override;

  // rendering
  bool STDCALL Draw(IGFXVertices *pVertices, IGFXIndices *pIndices) override;
  bool STDCALL DrawTemp() override;
  bool STDCALL DrawMesh(IGFXMesh *pMesh, const SHMatrix *matrices, int nNumMatrices) override;
  bool STDCALL DrawStringA(const char *pszString, int nX, int nY, DWORD dwColor) override;
  bool STDCALL DrawString(const wchar_t *pszString, int nX, int nY, DWORD dwColor) override;
  bool STDCALL DrawText(IGFXText *pText, const RECT &rect, int nY, DWORD dwFlags = FNT_FORMAT_LEFT) override;
  bool STDCALL DrawRects(const SGFXRect2 *pRects, int nNumRects, bool bSolid) override;

  // gamma ramp
  bool STDCALL SetGammaRamp(const SGFXGammaRamp &ramp, bool bCalibrate) override;
  bool STDCALL GetGammaRamp(const SGFXGammaRamp *pRamp) override;
  void STDCALL SetGammaCorrectionValues(float fBrightness, float fContrast, float fGamma) override;
  void STDCALL GetGammaCorrectionValues(float *pfBrightness, float *pfContrast, float *pfGamma) override;

  // screenshot
  bool STDCALL TakeScreenShot(interface IImage *pImage) override;

  // statistics
  int STDCALL GetNumPassedVertices() const override { return nNumPassedVertices; }
  int STDCALL GetNumPassedPrimitives() const override { return nNumPassedPrimitives; }

  // temporal function before shaders
  bool STDCALL SetShadingEffect(int nEffect) override;
};

#endif // __GRAPHICSENGINE_H__