#include "StdAfx.h"
#include "VideoCheck.h"
#include <d3d9.h>

#include "../Misc/Win32Helper.h"

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
  Zero(*pMemory);
  // use Direct3D9 to query available texture memory
  NWin32Helper::com_ptr<IDirect3D9> pD3D;
  pD3D.Create(Direct3DCreate9(D3D_SDK_VERSION));
  if (!pD3D) return false;
  // create a temporary device to query memory
  D3DPRESENT_PARAMETERS pp;
  Zero(pp);
  pp.Windowed = TRUE;
  pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
  pp.BackBufferFormat = D3DFMT_UNKNOWN;
  NWin32Helper::com_ptr<IDirect3DDevice9> pDevice;
  HRESULT dxrval = pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetDesktopWindow(),
                                      D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, pDevice.GetAddr());
  if (FAILED(dxrval)) return false;
  // GetAvailableTextureMem returns approximate total available texture memory in bytes
  const DWORD dwAvail = pDevice->GetAvailableTextureMem();
  pMemory->local.dwTotal = dwAvail;
  pMemory->local.dwFree = dwAvail;
  pMemory->texture.dwTotal = dwAvail;
  pMemory->texture.dwFree = dwAvail;

  return true;
}