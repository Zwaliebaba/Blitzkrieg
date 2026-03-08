#include "StdAfx.h"
#include "VideoCheck.h"
#include <ddraw.h>
#include <d3d9.h>

#include "../Misc/Win32Helper.h"

using DIRECTDRAWCREATEEX = HRESULT(WINAPI *)(GUID *, VOID **, REFIID, IUnknown *);

const wchar_t * STDCALL NVideoCheck::GetAPIName() { return L"DirectX"; }

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