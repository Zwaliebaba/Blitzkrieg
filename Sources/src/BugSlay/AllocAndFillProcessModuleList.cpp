/* ----------------------------------------------------------------------
        */
// //////////////////////////////////////////////////////////// 
#include "StdAfx.h"
#include "BugSlayer.h"
// The project internal header file.
#include "Internal.h"
// //////////////////////////////////////////////////////////// 
HMODULE * STDCALL AllocAndFillProcessModuleList(HANDLE hHeap, LPUINT puiCount)
{
  ASSERT(FALSE == IsBadWritePtr(puiCount , sizeof(LPUINT)));
  if (TRUE == IsBadWritePtr(puiCount, sizeof(LPUINT)))
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return nullptr;
  }

  ASSERT(NULL != hHeap) ;
  if (nullptr == hHeap)
  {
    SetLastError(ERROR_INVALID_PARAMETER);
    return nullptr;
  }

  // First, ask how many modules are really loaded.
  UINT uiQueryCount;

  BOOL bRet = GetLoadedModules(GetCurrentProcessId(), 0, nullptr, &uiQueryCount);
  ASSERT(TRUE == bRet);
  ASSERT(0 != uiQueryCount);

  if ((FALSE == bRet) || (0 == uiQueryCount)) return nullptr;

  // The HMODULE array.
  // Allocate the buffer to hold the returned array.
  auto pModArray = static_cast<HMODULE *>(HeapAlloc(hHeap, HEAP_ZERO_MEMORY, uiQueryCount * sizeof(HMODULE)));
  ASSERT(NULL != pModArray);
  if (nullptr == pModArray) return nullptr;

  // bRet holds BOOLEAN return values.
  bRet = GetLoadedModules(GetCurrentProcessId(), uiQueryCount, pModArray, puiCount);
  // Save off the last error so that the assert can still fire and
  // not change the value.
  DWORD dwLastError = GetLastError();
  ASSERT(TRUE == bRet);

  if (FALSE == bRet)
  {
    // Get rid of the last buffer.
    free(pModArray);
    pModArray = nullptr;
    SetLastError(dwLastError);
  }
  else SetLastError(ERROR_SUCCESS);
  // All OK, Jumpmaster!
  return pModArray;
}

// //////////////////////////////////////////////////////////// 