#include "StdAfx.h"

#include "../StreamIO/RandomGen.h"
#include "../Misc/Win32Helper.h"
#include "../Misc/FileUtils.h"
#include "../StreamIO/StreamIOTypes.h"

using GETSLS_HOOK = ISaveLoadSystem* (STDCALL *)();
using GETSINGLETONGLOBAL_HOOK = ISingleton* (STDCALL *)();
using GETTEMPRAWBUFFER_HOOK = void* (STDCALL *)(int nAmount, int nBufferIndex);

#ifdef _STREAMIO_DLL
ISaveLoadSystem * STDCALL GetSLS_Hook();
ISingleton * STDCALL GetSingletonGlobal_Hook();
void * STDCALL GetTempRawBuffer_Hook(int nSize, int nIndex);
#endif // _STREAMIO_DLL

IRandomGen *g_pGlobalRandomGen = nullptr;
ISaveLoadSystem *g_pGlobalSaveLoadSystem = nullptr;
ISingleton *g_pGlobalSingleton = nullptr;
GETTEMPRAWBUFFER_HOOK g_pfnGlobalGetTempRawBuffer = nullptr;

static std::list<NWin32Helper::CDLLHandle *> dllhandles;

NWin32Helper::CDLLHandle *LoadModule(const char *pszName)
{
  auto pDLL = new NWin32Helper::CDLLHandle(pszName);
  if (!pDLL->IsLoaded())
  {
    delete pDLL;
    return nullptr;
  }
  return pDLL;
}

class CGlobalsLoader
{
public:
  CGlobalsLoader()
  {
    // load StreamIO.dll
#ifndef _DONT_LOAD_STREAMIO
    NWin32Helper::CDLLHandle *pStreamIO = LoadModule("streamio.dll");
    // hook GetGlobalSaveLoadSystem() function
    if (GETSLS_HOOK pfnGetSLS_Hook = pStreamIO->GetProcAddress("GetSLS_Hook", (GETSLS_HOOK) 0)) g_pGlobalSaveLoadSystem = (*pfnGetSLS_Hook)();
    // hook GetSingletonGlobal() function
    if (GETSINGLETONGLOBAL_HOOK pfnGetSingletonGlobal_Hook = pStreamIO->GetProcAddress("GetSingletonGlobal_Hook", (GETSINGLETONGLOBAL_HOOK) 0)) g_pGlobalSingleton = (*pfnGetSingletonGlobal_Hook)();
    // hook GetTempRawBuffer() function
    g_pfnGlobalGetTempRawBuffer = pStreamIO->GetProcAddress("GetTempRawBuffer_Hook", (GETTEMPRAWBUFFER_HOOK) 0);

    dllhandles.push_back(pStreamIO);
#endif // _DONT_LOAD_STREAMIO
    //
#ifdef _STREAMIO_DLL
    g_pGlobalSaveLoadSystem = GetSLS_Hook();
    g_pGlobalSingleton = GetSingletonGlobal_Hook();
    g_pfnGlobalGetTempRawBuffer = GetTempRawBuffer_Hook;
#endif // _STREAMIO_DLL
    //
    //
#ifndef _DONT_LOAD_SINGLETONS
    ISingleton *pSingleton = GetSingletonGlobal();
    if (pSingleton) { g_pGlobalRandomGen = GetSingleton<IRandomGen>(pSingleton); }
#endif // _DONT_LOAD_SINGLETONS
  }

  //
  //
  //
  ~CGlobalsLoader()
  {
    for (auto it = dllhandles.begin(); it != dllhandles.end(); ++it) delete (*it);
    dllhandles.clear();
  }
};

static CGlobalsLoader theGlobalsLoader;