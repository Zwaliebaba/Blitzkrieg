#include "StdAfx.h"

using GETTEMPRAWBUFFER_HOOK = void* (STDCALL *)(int nAmount, int nBufferIndex);

ISaveLoadSystem * STDCALL GetSLS_Hook();
ISingleton * STDCALL GetSingletonGlobal_Hook();
void * STDCALL GetTempRawBuffer_Hook(int nSize, int nIndex);

interface IRandomGen;
IRandomGen *g_pGlobalRandomGen = nullptr;
ISaveLoadSystem *g_pGlobalSaveLoadSystem = nullptr;
ISingleton *g_pGlobalSingleton = nullptr;
GETTEMPRAWBUFFER_HOOK g_pfnGlobalGetTempRawBuffer = nullptr;

struct CGlobalsLoader
{
  CGlobalsLoader()
  {
	g_pGlobalSaveLoadSystem = GetSLS_Hook();
	g_pGlobalSingleton = GetSingletonGlobal_Hook();
	g_pfnGlobalGetTempRawBuffer = GetTempRawBuffer_Hook;
	// NOTE: Do NOT call GetSingleton<IRandomGen>() here.
	// g_pGlobalRandomGen is initialized in NMain::Initialize() instead,
	// after the singleton registry is fully populated.
  }
};

static CGlobalsLoader theGlobalsLoader;

