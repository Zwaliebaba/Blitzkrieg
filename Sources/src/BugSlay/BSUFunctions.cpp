/* ----------------------------------------------------------------------
        */
// //////////////////////////////////////////////////////////// 
#include "StdAfx.h"
#include "BugSlayer.h"
#include "Internal.h"
// //////////////////////////////////////////////////////////// 
// The Win95 version of GetModuleBaseName.
static DWORD STDCALL Win95GetModuleBaseName(HANDLE hProcess, HMODULE hModule,
                                            LPSTR lpBaseName, DWORD nSize);
// //////////////////////////////////////////////////////////// 
/* ////////////////////////////////////////////////////////////////////
                         */
DWORD STDCALL BSUGetModuleBaseName(HANDLE hProcess, HMODULE hModule, LPTSTR lpBaseName, DWORD nSize)
{
  // Call the NT version.  
  // where all the PSAPI wrappers are kept.
  if (TRUE == IsNT()) return NTGetModuleBaseName(hProcess, hModule, lpBaseName, nSize);
  return Win95GetModuleBaseName(hProcess, hModule, lpBaseName, nSize);
}

// //////////////////////////////////////////////////////////// 
static DWORD STDCALL Win95GetModuleBaseName(HANDLE /* hProcess */, HMODULE hModule,
                                            LPSTR lpBaseName, DWORD nSize)
{
  ASSERT(FALSE == IsBadWritePtr(lpBaseName, nSize));
  if (TRUE == IsBadWritePtr(lpBaseName, nSize))
  {
    TRACE0("Win95GetModuleBaseName Invalid string buffer\n");
    SetLastError(ERROR_INVALID_PARAMETER);
    return 0;
  }
  // This could blow the stack...
  char szBuff[MAX_PATH + 1];
  DWORD dwRet = GetModuleFileName(hModule, szBuff, MAX_PATH);
  ASSERT(0 != dwRet) ;
  if (0 == dwRet) return 0;

  // Find the last '\' mark.
  char *pStart = strrchr(szBuff, '\\');
  int iMin;
  if (nullptr != pStart)
  {
    // Move up one character.
    pStart++;
    // lint-e666
    iMin = Min(static_cast<int>(nSize), lstrlen(pStart) + 1);
    // lint +e666
    lstrcpyn(lpBaseName, pStart, iMin);
  }
  else
  {
    // Copy the szBuff buffer in.
    // lint-e666
    iMin = Min(static_cast<int>(nSize), lstrlen(szBuff) + 1);
    // lint +e666
    lstrcpyn(lpBaseName, szBuff, iMin);
  }
  // Always NULL terminate.
  lpBaseName[iMin] = '\0';
  return static_cast<DWORD>(iMin - 1);
}

// //////////////////////////////////////////////////////////// 