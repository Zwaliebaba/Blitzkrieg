#include "StdAfx.h"

#include "VideoCheck.h"

#include <ddraw.h>
#include <dinput.h>
#include <dmusici.h>
#include <d3d8.h>

#include "../Misc/Win32Helper.h"

using DIRECTDRAWCREATE = HRESULT(WINAPI *)(GUID *, LPDIRECTDRAW *, IUnknown *);
using DIRECTDRAWCREATEEX = HRESULT(WINAPI *)(GUID *, VOID **, REFIID, IUnknown *);
using DIRECTINPUTCREATE = HRESULT(WINAPI *)(HINSTANCE, DWORD, LPDIRECTINPUT *, IUnknown *);
using DIRECT3DCREATE8 = IDirect3D8*(WINAPI *)(UINT SDKVersion);

const wchar_t * STDCALL NVideoCheck::GetAPIName() { return L"DirectX"; }

// This function returns the DirectX version number as follows:
// 0x0000 = No DirectX installed
// 0x0100 = DirectX version 1 installed
// 0x0200 = DirectX 2 installed
// 0x0300 = DirectX 3 installed
// 0x0500 = At ​​least DirectX 5 installed.
// 0x0600 = At ​​least DirectX 6 installed.
// 0x0601 = At ​​least DirectX 6.1 installed.
// 0x0700 = At ​​least DirectX 7 installed.
// 0x0800 = At ​​least DirectX 8 installed.
// 
// Please note that this code is intended as a general guideline. 
// app will probably be able to simply query for functionality (via
// QueryInterface) for one or two components.
//
// Please also note:
// "if( dwDXVersion != 0x500 ) return FALSE;" 
// "if( dwDXVersion < 0x500 ) return FALSE;" 
// to ensure your app will run on future releases of DirectX.

DWORD STDCALL NVideoCheck::GetAPIVersion()
{
  HINSTANCE hDDrawDLL = nullptr;
  HINSTANCE hDInputDLL = nullptr;
  HINSTANCE hD3D8DLL = nullptr;
  LPDIRECTDRAW pDDraw = nullptr;
  LPDIRECTDRAW2 pDDraw2 = nullptr;
  LPDIRECTDRAWSURFACE pSurf = nullptr;
  LPDIRECTDRAWSURFACE3 pSurf3 = nullptr;
  LPDIRECTDRAWSURFACE4 pSurf4 = nullptr;
  DWORD dwDXVersion = 0;

  // First see if DDRAW.DLL even exists.
  hDDrawDLL = LoadLibrary("DDRAW.DLL");
  if (hDDrawDLL == nullptr)
  {
    dwDXVersion = 0;
    return dwDXVersion;
  }

  // See if we can create the DirectDraw object.
  auto DirectDrawCreate = (DIRECTDRAWCREATE) GetProcAddress(hDDrawDLL, "DirectDrawCreate");
  if (DirectDrawCreate == nullptr)
  {
    dwDXVersion = 0;
    FreeLibrary(hDDrawDLL);
    OutputDebugString("Couldn't LoadLibrary DDraw\r\n");
    return dwDXVersion;
  }

  HRESULT hr = DirectDrawCreate(nullptr, &pDDraw, nullptr);
  if (FAILED(hr))
  {
    dwDXVersion = 0;
    FreeLibrary(hDDrawDLL);
    OutputDebugString("Couldn't create DDraw\r\n");
    return dwDXVersion;
  }

  // So DirectDraw exists.  
  dwDXVersion = 0x100;

  // Let's see if IID_IDirectDraw2 exists.
  hr = pDDraw->QueryInterface(IID_IDirectDraw2, (VOID **) &pDDraw2);
  if (FAILED(hr))
  {
    // No IDirectDraw2 exists... must be DX1
    pDDraw->Release();
    FreeLibrary(hDDrawDLL);
    OutputDebugString("Couldn't QI DDraw2\r\n");
    return dwDXVersion;
  }

  // IDirectDraw2 exists. 
  pDDraw2->Release();
  dwDXVersion = 0x200;

  // -------------------------------------------------------------------------
  // DirectX 3.0 Checks
  // -------------------------------------------------------------------------

  // DirectInput was added for DX3
  hDInputDLL = LoadLibrary("DINPUT.DLL");
  if (hDInputDLL == nullptr)
  {
    // No DInput...must not be DX3
    OutputDebugString("Couldn't LoadLibrary DInput\r\n");
    pDDraw->Release();
    return dwDXVersion;
  }

  auto DirectInputCreate = (DIRECTINPUTCREATE) GetProcAddress(hDInputDLL, "DirectInputCreateA");
  if (DirectInputCreate == nullptr)
  {
    // No DInput...must be DX2
    FreeLibrary(hDInputDLL);
    FreeLibrary(hDDrawDLL);
    pDDraw->Release();
    OutputDebugString("Couldn't GetProcAddress DInputCreate\r\n");
    return dwDXVersion;
  }

  // DirectInputCreate exists. 
  dwDXVersion = 0x300;
  FreeLibrary(hDInputDLL);

  // Can do checks for 3a vs 3b here

  // -------------------------------------------------------------------------
  // DirectX 5.0 Checks
  // -------------------------------------------------------------------------

  // We can tell if DX5 is present by checking for the existence of
  // IDirectDrawSurface3. 
  DDSURFACEDESC ddsd;
  ZeroMemory(&ddsd, sizeof(ddsd));
  ddsd.dwSize = sizeof(ddsd);
  ddsd.dwFlags = DDSD_CAPS;
  ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

  hr = pDDraw->SetCooperativeLevel(nullptr, DDSCL_NORMAL);
  if (FAILED(hr))
  {
    // Failure. 
    pDDraw->Release();
    FreeLibrary(hDDrawDLL);
    dwDXVersion = 0;
    OutputDebugString("Couldn't Set coop level\r\n");
    return dwDXVersion;
  }

  hr = pDDraw->CreateSurface(&ddsd, &pSurf, nullptr);
  if (FAILED(hr))
  {
    // Failure. 
    pDDraw->Release();
    FreeLibrary(hDDrawDLL);
    dwDXVersion = 0;
    OutputDebugString("Couldn't CreateSurface\r\n");
    return dwDXVersion;
  }

  // Query for the IDirectDrawSurface3 interface
  if (FAILED(pSurf->QueryInterface( IID_IDirectDrawSurface3, (VOID**)&pSurf3 )))
  {
    pDDraw->Release();
    FreeLibrary(hDDrawDLL);
    return dwDXVersion;
  }

  // QI for IDirectDrawSurface3 succeeded. 
  dwDXVersion = 0x500;

  // -------------------------------------------------------------------------
  // DirectX 6.0 Checks
  // -------------------------------------------------------------------------

  // The IDirectDrawSurface4 interface was introduced with DX 6.0
  if (FAILED(pSurf->QueryInterface( IID_IDirectDrawSurface4, (VOID**)&pSurf4 )))
  {
    pDDraw->Release();
    FreeLibrary(hDDrawDLL);
    return dwDXVersion;
  }

  // IDirectDrawSurface4 was created successfully. 
  dwDXVersion = 0x600;
  pSurf->Release();
  pDDraw->Release();

  // -------------------------------------------------------------------------
  // DirectX 6.1 Checks
  // -------------------------------------------------------------------------

  // Check for DMusic, which was introduced with DX6.1
  LPDIRECTMUSIC pDMusic = NULL;
  CoInitialize(nullptr);
  hr = CoCreateInstance(CLSID_DirectMusic, nullptr, CLSCTX_INPROC_SERVER, IID_IDirectMusic, static_cast<void **>(&pDMusic));
  if (FAILED(hr))
  {
    OutputDebugString("Couldn't create CLSID_DirectMusic\r\n");
    FreeLibrary(hDDrawDLL);
    return dwDXVersion;
  }

  // DirectMusic was created successfully. 
  dwDXVersion = 0x601;
  pDMusic->Release();
  CoUninitialize();

  // -------------------------------------------------------------------------
  // DirectX 7.0 Checks
  // -------------------------------------------------------------------------

  // Check for DirectX 7 by creating a DDraw7 object
  LPDIRECTDRAW7 pDD7;
  auto DirectDrawCreateEx = (DIRECTDRAWCREATEEX) GetProcAddress(hDDrawDLL, "DirectDrawCreateEx");
  if (nullptr == DirectDrawCreateEx)
  {
    FreeLibrary(hDDrawDLL);
    return dwDXVersion;
  }

  if (FAILED(DirectDrawCreateEx( NULL, (VOID**)&pDD7, IID_IDirectDraw7, NULL )))
  {
    FreeLibrary(hDDrawDLL);
    return dwDXVersion;
  }

  // DDraw7 was created successfully. 
  dwDXVersion = 0x700;
  pDD7->Release();

  // -------------------------------------------------------------------------
  // DirectX 8.0 Checks
  // -------------------------------------------------------------------------

  // Simply see if D3D8.dll exists.
  hD3D8DLL = LoadLibrary("D3D8.DLL");
  if (hD3D8DLL == nullptr)
  {
    FreeLibrary(hDDrawDLL);
    return dwDXVersion;
  }

  // D3D8.dll exists. 
  dwDXVersion = 0x800;

  // -------------------------------------------------------------------------
  // End of checking for versions of DirectX
  // -------------------------------------------------------------------------

  // Close open libraries and return
  FreeLibrary(hDDrawDLL);
  FreeLibrary(hD3D8DLL);

  return dwDXVersion;
}

// ************************************************************************************************************************ //
// **
// ** get video limits in accordance with memory amount
// **
// **
// **
// ************************************************************************************************************************ //

bool STDCALL NVideoCheck::GetVideoMemory(SVideoMemory *pMemory)
{
  // create DirectDraw7 object to get available video memory
  NWin32Helper::CDLLHandle handle("ddraw.dll");
  if (!handle.IsLoaded()) return false;
  DIRECTDRAWCREATEEX pfnDirectDrawCreateEx = handle.GetProcAddress("DirectDrawCreateEx", static_cast<DIRECTDRAWCREATEEX>(nullptr));
  if (pfnDirectDrawCreateEx == nullptr) return false;
  LPDIRECTDRAW7 pDD7Temp = nullptr;
  HRESULT dxrval = (*pfnDirectDrawCreateEx)(nullptr, (void **) &pDD7Temp, IID_IDirectDraw7, nullptr);
  if (FAILED(dxrval)) return false;
  NWin32Helper::com_ptr<IDirectDraw7> pDD = pDD7Temp;
  pDD7Temp->Release();
  // check for total and free video memory
  Zero(*pMemory);
  DWORD dwTotal = 0, dwFree = 0;
  DDSCAPS2 caps;
  Zero(caps);
  // local video memory
  caps.dwCaps = DDSCAPS_VIDEOMEMORY;
  dxrval = pDD->GetAvailableVidMem(&caps, &dwTotal, &dwFree);
  if (SUCCEEDED(dxrval))
  {
    pMemory->local.dwTotal = dwTotal;
    pMemory->local.dwFree = dwFree;
  }
  // non-local (AGP) video memory
  caps.dwCaps = DDSCAPS_NONLOCALVIDMEM;
  dxrval = pDD->GetAvailableVidMem(&caps, &dwTotal, &dwFree);
  if (SUCCEEDED(dxrval))
  {
    pMemory->nonlocal.dwTotal = dwTotal;
    pMemory->nonlocal.dwFree = dwFree;
  }
  // texture memory
  caps.dwCaps = DDSCAPS_TEXTURE;
  pDD->GetAvailableVidMem(&caps, &dwTotal, &dwFree);
  if (SUCCEEDED(dxrval))
  {
    pMemory->texture.dwTotal = dwTotal;
    pMemory->texture.dwFree = dwFree;
  }

  return true;
}