/* ----------------------------------------------------------------------
        */

#include "StdAfx.h"
#include "BugSlayer.h"
#include "Internal.h"

/* ////////////////////////////////////////////////////////////////////
                                 */
// The typedefs for the PSAPI.DLL functions used by this module.
using ENUMPROCESSMODULES = BOOL(WINAPI *)(HANDLE hProcess,
                                          HMODULE *lphModule,
                                          DWORD cb,
                                          LPDWORD lpcbNeeded);

using GETMODULEBASENAME = DWORD(WINAPI *)(HANDLE hProcess,
                                          HMODULE hModule,
                                          LPTSTR lpBaseName,
                                          DWORD nSize);


/* ////////////////////////////////////////////////////////////////////
                             */
// Has the function stuff here been initialized?  
// used by the InitPSAPI function and nothing else.
static BOOL g_bInitialized = FALSE;
// The pointer to EnumProcessModules.
static ENUMPROCESSMODULES g_pEnumProcessModules = nullptr;
// The pointer to GetModuleBaseName.
static GETMODULEBASENAME g_pGetModuleBaseName = nullptr;

/* ----------------------------------------------------------------------
 */
static BOOL InitPSAPI(void)
{
  if (TRUE == g_bInitialized) return TRUE;

  // Load up PSAPI.DLL.
  HINSTANCE hInst = LoadLibraryA("PSAPI.DLL");
  ASSERT(NULL != hInst);
  if (nullptr == hInst)
  {
    TRACE0("Unable to load PSAPI.DLL!\n");
    return FALSE;
  }

  // Now do the GetProcAddress stuff.
  g_pEnumProcessModules = (ENUMPROCESSMODULES) GetProcAddress(hInst, "EnumProcessModules");
  ASSERT(NULL != g_pEnumProcessModules);
  if (nullptr == g_pEnumProcessModules)
  {
    TRACE0("GetProcAddress failed on EnumProcessModules!\n");
    return FALSE;
  }

  g_pGetModuleBaseName = (GETMODULEBASENAME) GetProcAddress(hInst, "GetModuleBaseNameA");
  ASSERT(NULL != g_pGetModuleBaseName);
  if (nullptr == g_pGetModuleBaseName)
  {
    TRACE0("GetProcAddress failed on GetModuleBaseNameA!\n");
    return FALSE;
  }

  // All OK, Jumpmaster!
  g_bInitialized = TRUE;
  return TRUE;
}

/* ----------------------------------------------------------------------
 */
BOOL NT4GetLoadedModules(DWORD dwPID, UINT uiCount, HMODULE *paModArray, LPUINT puiRealCount)
{
  // Initialize PSAPI.DLL, if needed.
  if (FALSE == InitPSAPI())
  {
    ASSERT(FALSE);
    SetLastErrorEx(ERROR_DLL_INIT_FAILED, SLE_ERROR);
    return FALSE;
  }

  // Convert the process ID into a process handle.
  HANDLE hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, dwPID);
  ASSERT(NULL != hProc);
  if (nullptr == hProc)
  {
    TRACE1("Unable to OpenProcess on %08X\n", dwPID);
    return FALSE;
  }

  // Now get the modules for the specified process.
  ASSERT(NULL != g_pEnumProcessModules);
  // Because of possible DLL unload order differences, make sure that
  // PSAPI.DLL is still loaded in case this function is called during
  // shutdown
  ASSERT(FALSE == IsBadCodePtr((FARPROC)g_pEnumProcessModules));
  if (TRUE == IsBadCodePtr((FARPROC) g_pEnumProcessModules))
  {
    TRACE0("PSAPI.DLL has been unloaded on us!\n");
    // Close the process handle used.
    VERIFY(CloseHandle(hProc));

    SetLastErrorEx(ERROR_INVALID_DLL, SLE_ERROR);

    return FALSE;
  }

  DWORD dwTotal = 0;
  BOOL bRet = (*g_pEnumProcessModules)(hProc, paModArray, uiCount * sizeof(HMODULE), &dwTotal);

  // Close the process handle used.
  VERIFY(CloseHandle(hProc));

  // Convert the count from bytes to HMODULE values.
  *puiRealCount = dwTotal / sizeof(HMODULE);

  // If bRet was FALSE, and the user was not just asking for the
  // total, there was a problem.
  if (((FALSE == bRet) && (uiCount > 0)) || (0 == dwTotal))
  {
    ASSERT(FALSE);
    TRACE0("EnumProcessModules failed!\n");
    return FALSE;
  }

  // If the total returned in puiRealCount is larger than the value in
  // uiCount, then return an error.  
  // not an error.
  if ((*puiRealCount > uiCount) && (uiCount > 0))
  {
    ASSERT(FALSE);
    TRACE0("Buffer is too small in NT4GetLoadedModules!\n");
    SetLastErrorEx(ERROR_INSUFFICIENT_BUFFER, SLE_ERROR);
    return FALSE;
  }

  // All OK, Jumpmaster!
  SetLastError(ERROR_SUCCESS);
  return TRUE;
}

DWORD STDCALL NTGetModuleBaseName(HANDLE hProcess, HMODULE hModule, LPTSTR lpBaseName, DWORD nSize)
{
  // Initialize PSAPI.DLL, if needed.
  if (FALSE == InitPSAPI())
  {
    ASSERT(FALSE);
    SetLastErrorEx(ERROR_DLL_INIT_FAILED, SLE_ERROR);
    return FALSE;
  }
  return (*g_pGetModuleBaseName)(hProcess, hModule, lpBaseName, nSize);
}