/* ----------------------------------------------------------------------
        */

#ifndef _SYMBOLENGINE_H
#define _SYMBOLENGINE_H
// //////////////////////////////////////////////////////////// 
#include <imagehlp.h>
#include <tchar.h>
// //////////////////////////////////////////////////////////// 
// Include these in case the user forgets to link against them.
#pragma comment( lib, "imagehlp.lib" )
#pragma comment( lib, "version.lib" )
// //////////////////////////////////////////////////////////// 
// The API_VERSION_NUMBER define is 5 with the NT4 IMAGEHLP.H.  
// 7 with the November Platform SDK version.  
// reliable way to see which header is being used.
#if ( API_VERSION_NUMBER < 7 )
#define SYMENG_EXTRAWORK
#define SYMENG_NEEDDECLS
#else
#undef SYMENG_EXTRAWORK
#undef SYMENG_NEEDDECLS
#endif

// If FLEXIBLE_SYMBOLENGINE is defined, ALWAYS define SYMENG_EXTRAWORK
#ifdef FLEXIBLE_SYMBOLENGINE
#define SYMENG_EXTRAWORK
#endif // FLEXIBLE_SYMBOLENGINE

// I will define the structures that need defining in case the included
// IMAGEHLP.H is from NT4.0.

#ifdef SYMENG_NEEDDECLS
// source file line data structure
typedef struct _IMAGEHLP_LINE
{
  DWORD SizeOfStruct;// set to sizeof(IMAGEHLP_LINE)
  DWORD Key;// internal
  DWORD LineNumber;// line number in file
  PCHAR FileName;// full filename
  DWORD Address;// first instruction of line
} IMAGEHLP_LINE, *PIMAGEHLP_LINE;
#endif  // SYMENG_NEEDDECLS

#ifndef SYMOPT_LOAD_LINES
#define SYMOPT_LOAD_LINES        0x00000010
#endif  // SYMOPT_LOAD_LINES

#ifndef SYMOPT_OMAP_FIND_NEAREST
#define SYMOPT_OMAP_FIND_NEAREST 0x00000020
#endif  // SYMOPT_OMAP_FIND_NEAREST
// //////////////////////////////////////////////////////////// 
/* I just lifted the prototypes right out of the NT5 header.

 */
// //////////////////////////////////////////////////////////// 
#ifdef SYMENG_NEEDDECLS
BOOL STDCALL SymGetLineFromAddr(IN HANDLE hProcess,
                                IN DWORD  dwAddr,
                                OUT PDWORD pdwDisplacement,
                                OUT PIMAGEHLP_LINE Line);

BOOL STDCALL SymGetLineFromName(IN HANDLE hProcess,
                                IN LPSTR ModuleName,
                                IN LPSTR FileName,
                                IN DWORD dwLineNumber,
                                OUT PLONG plDisplacement,
                                IN OUT PIMAGEHLP_LINE Line);

BOOL STDCALL SymGetLineNext(IN HANDLE hProcess,
                            IN OUT PIMAGEHLP_LINE Line);

BOOL STDCALL SymGetLinePrev(IN HANDLE hProcess,
                            IN OUT PIMAGEHLP_LINE Line);

BOOL STDCALL SymMatchFileName(IN LPSTR FileName,
                              IN LPSTR Match,
                              OUT LPSTR *FileNameStop,
                              OUT LPSTR *MatchStop);
#endif  // SYMENG_NEEDDECLS
// //////////////////////////////////////////////////////////// 
// The great Bugslayer idea of ​​creating wrapper classes on structures that have size fields came from
// fellow MSJ columnist, Paul DiLascia.
// Thanks, Paul!

// I did not wrap IMAGEHLP_SYMBOL because that is a variable size
// structure.

// //////////////////////////////////////////////////////////// 
// The IMAGEHLP_MODULE wrapper class.
struct CImageHlp_Module : IMAGEHLP_MODULE
{
  CImageHlp_Module()
  {
    memset(this, 0, sizeof(IMAGEHLP_MODULE));
    SizeOfStruct = sizeof(IMAGEHLP_MODULE);
  }
};

// The IMAGEHLP_LINE wrapper class.
struct CImageHlp_Line : IMAGEHLP_LINE
{
  CImageHlp_Line()
  {
    memset(this, 0, sizeof(IMAGEHLP_LINE));
    SizeOfStruct = sizeof(IMAGEHLP_LINE);
  }
};

// //////////////////////////////////////////////////////////// 
// Typedefs for the new source and line functions.
using PFNSYMGETLINEFROMADDR = BOOL(STDCALL *)(IN HANDLE hProcess,
                                              IN DWORD dwAddr,
                                              OUT PDWORD pdwDisplacement,
                                              OUT PIMAGEHLP_LINE Line);

using PFNSYMGETLINEFROMNAME = BOOL(STDCALL *)(IN HANDLE hProcess,
                                              IN LPSTR ModuleName,
                                              IN LPSTR FileName,
                                              IN DWORD dwLineNumber,
                                              OUT PLONG plDisplacement,
                                              IN OUT PIMAGEHLP_LINE Line);

using PFNSYMGETLINENEXT = BOOL(STDCALL *)(IN HANDLE hProcess,
                                          IN OUT PIMAGEHLP_LINE Line);

using PFNSYMGETLINEPREV = BOOL(STDCALL *)(IN HANDLE hProcess,
                                          IN OUT PIMAGEHLP_LINE Line);

using PFNSYMMATCHFILENAME = BOOL(STDCALL *)(IN LPSTR FileName,
                                            IN LPSTR Match,
                                            OUT LPSTR *FileNameStop,
                                            OUT LPSTR *MatchStop);

// //////////////////////////////////////////////////////////// 
// The symbol engine class.
class CSymbolEngine
{
  /* ----------------------------------------------------------------------
                     */
public:
  // The constructor just does the work of finding the new functions.
  // Use this class just line the normal C functions and call the
  // SymInitialize function to get everything started.
  CSymbolEngine()
  {
#ifdef SYMENG_EXTRAWORK
    HINSTANCE hInstImageHlp = GetModuleHandleA("IMAGEHLP.DLL");

    m_pfnSymGetLineFromAddr = (PFNSYMGETLINEFROMADDR) GetProcAddress(hInstImageHlp, "SymGetLineFromAddr");
    if (nullptr == m_pfnSymGetLineFromAddr)
    {
      m_pfnSymGetLineFromName = nullptr;
      m_pfnSymGetLineNext = nullptr;
      m_pfnSymGetLinePrev = nullptr;
      m_pfnSymMatchFileName = nullptr;
    }
    else
    {
      m_pfnSymGetLineFromName = (PFNSYMGETLINEFROMNAME) GetProcAddress(hInstImageHlp, "SymGetLineFromName");
      m_pfnSymGetLineNext = (PFNSYMGETLINENEXT) GetProcAddress(hInstImageHlp, "SymGetLineNext");
      m_pfnSymGetLinePrev = (PFNSYMGETLINEPREV) GetProcAddress(hInstImageHlp, "SymGetLinePrev");
      m_pfnSymMatchFileName = (PFNSYMMATCHFILENAME) GetProcAddress(hInstImageHlp, "SymMatchFileName");
    }
#endif  // SYMENG_EXTRAWORK
  }

  virtual ~CSymbolEngine() {}

  /* ----------------------------------------------------------------------
                   */

  BOOL CanDoSourceLines()
  {
#ifdef SYMENG_EXTRAWORK
    return (nullptr != m_pfnSymGetLineFromAddr);
#else
    return TRUE;
#endif  // SYMENG_EXTRAWORK
  }

  // Returns the file version string of IMAGEHLP.DLL being used.
  // To convert the return values ​​into something readable:
  // wsprintf(szVer,
  // _T("%d.%02d.%d.%d"),
  // HIWORD (dwMS),
  // LOWORD(dwMS),
  // HIWORD(dwLS),
  // LOWORD(dwLS));
  // szVer will contain a string like: 5.00.1878.1
  BOOL GetImageHlpVersion(DWORD &dwMS, DWORD &dwLS) { return GetInMemoryFileVersion("IMAGEHLP.DLL", dwMS, dwLS); }

  // Does the same thing for the PDB reading DLLs.
  BOOL GetPDBReaderVersion(DWORD &dwMS, DWORD &dwLS)
  {
    // First try MSDBI.DLL.
    if (TRUE == GetInMemoryFileVersion("MSDBI.DLL", dwMS, dwLS)) return TRUE;
    // MSDBI.DLL is not in memory so try MSPDB50.DLL.
    return GetInMemoryFileVersion("MSPDB50.DLL", dwMS, dwLS);
  }

  // The worker function used by the previous two functions.
  BOOL GetInMemoryFileVersion(LPCTSTR szFile, DWORD &dwMS, DWORD &dwLS)
  {
    HMODULE hInstIH = GetModuleHandle(szFile);

    // Get the full filename of the loaded version.
    TCHAR szImageHlp[MAX_PATH];
    GetModuleFileName(hInstIH, szImageHlp, MAX_PATH);

    dwMS = 0;
    dwLS = 0;

    // Get the version info size.
    DWORD dwVerInfoHandle;
    DWORD dwVerSize = GetFileVersionInfoSize(szImageHlp, &dwVerInfoHandle);
    if (0 == dwVerSize) return FALSE;

    // Got the version size, now get the version info.
    auto lpData = new TCHAR[dwVerSize];
    if (FALSE == GetFileVersionInfo(szImageHlp, dwVerInfoHandle, dwVerSize, lpData))
    {
      delete []lpData;
      return FALSE;
    }

    VS_FIXEDFILEINFO *lpVerInfo = nullptr;
    UINT uiLen;
    BOOL bRet = VerQueryValue(lpData, _T("\\"), (LPVOID *) &lpVerInfo, &uiLen);
    if (TRUE == bRet)
    {
      dwMS = lpVerInfo->dwFileVersionMS;
      dwLS = lpVerInfo->dwFileVersionLS;
    }

    delete []lpData;

    return bRet;
  }

  /* ----------------------------------------------------------------------
                    */

  BOOL SymInitialize(IN HANDLE hProcess,
                     IN LPSTR UserSearchPath,
                     IN BOOL fInvadeProcess)
  {
    m_hProcess = hProcess;
    return ::SymInitialize(hProcess, UserSearchPath, fInvadeProcess);
  }

  BOOL SymCleanup() { return ::SymCleanup(m_hProcess); }

  /* ----------------------------------------------------------------------
                        */

  BOOL SymEnumerateModules(IN PSYM_ENUMMODULES_CALLBACK EnumModulesCallback,
                           IN PVOID UserContext) { return ::SymEnumerateModules(m_hProcess, EnumModulesCallback, UserContext); }

  BOOL SymLoadModule(IN HANDLE hFile,
                     IN PSTR ImageName,
                     IN PSTR ModuleName,
                     IN DWORD BaseOfDll,
                     IN DWORD SizeOfDll) { return ::SymLoadModule(m_hProcess, hFile, ImageName, ModuleName, BaseOfDll, SizeOfDll); }

  BOOL EnumerateLoadedModules(IN PENUMLOADED_MODULES_CALLBACK EnumLoadedModulesCallback,
                              IN PVOID UserContext) { return ::EnumerateLoadedModules(m_hProcess, EnumLoadedModulesCallback, UserContext); }

  BOOL SymUnloadModule(IN DWORD BaseOfDll) { return ::SymUnloadModule(m_hProcess, BaseOfDll); }

  BOOL SymGetModuleInfo(IN DWORD dwAddr,
                        OUT PIMAGEHLP_MODULE ModuleInfo) { return ::SymGetModuleInfo(m_hProcess, dwAddr, ModuleInfo); }

  DWORD SymGetModuleBase(IN DWORD dwAddr) { return ::SymGetModuleBase(m_hProcess, dwAddr); }

  /* ----------------------------------------------------------------------
                        */

  BOOL SymEnumerateSymbols(IN DWORD BaseOfDll,
                           IN PSYM_ENUMSYMBOLS_CALLBACK EnumSymbolsCallback,
                           IN PVOID UserContext) { return ::SymEnumerateSymbols(m_hProcess, BaseOfDll, EnumSymbolsCallback, UserContext); }

  BOOL SymGetSymFromAddr(IN DWORD dwAddr,
                         OUT PDWORD pdwDisplacement,
                         OUT PIMAGEHLP_SYMBOL Symbol) { return ::SymGetSymFromAddr(m_hProcess, dwAddr, pdwDisplacement, Symbol); }

  BOOL SymGetSymFromName(IN LPSTR Name,
                         OUT PIMAGEHLP_SYMBOL Symbol) { return ::SymGetSymFromName(m_hProcess, Name, Symbol); }

  BOOL SymGetSymNext(IN OUT PIMAGEHLP_SYMBOL Symbol) { return ::SymGetSymNext(m_hProcess, Symbol); }

  BOOL SymGetSymPrev(IN OUT PIMAGEHLP_SYMBOL Symbol) { return ::SymGetSymPrev(m_hProcess, Symbol); }

  /* ----------------------------------------------------------------------
                     */

  BOOL SymGetLineFromAddr(IN DWORD dwAddr,
                          OUT PDWORD pdwDisplacement,
                          OUT PIMAGEHLP_LINE Line)
  {
    // Holds the function pointer.
    PFNSYMGETLINEFROMADDR pfnSGLFA;

#ifdef SYMENG_EXTRAWORK
    if (nullptr == m_pfnSymGetLineFromAddr) return FALSE;
    pfnSGLFA = m_pfnSymGetLineFromAddr;
#else
    // Just grab it directly.
    pfnSGLFA = ::SymGetLineFromAddr;
#endif  // SYMENG_EXTRAWORK

#ifdef DO_NOT_WORK_AROUND_SRCLINE_BUG
    // Just return whatever the main function returned.
    return (*pfnSGLFA)(m_hProcess, dwAddr, pdwDisplacement, Line);

#else
    // The problem is that the symbol engine only finds those source
    // line addresses (after the first lookup) that fall exactly on
    // a zero displacement.  
    // find the line and return the proper displacement.
    DWORD dwTempDis = 0;
    while (FALSE == (*pfnSGLFA)(m_hProcess, dwAddr - dwTempDis, pdwDisplacement, Line))
    {
      dwTempDis += 1;
      if (100 == dwTempDis) return FALSE;
    }
    // It was found and the source line information is correct so
    // change the displacement if it was looked up multiple times.
    if (0 != dwTempDis) *pdwDisplacement = dwTempDis;
    return TRUE;
#endif // DO_NOT_WORK_AROUND_SRCLINE_BUG
  }

  BOOL SymGetLineFromName(IN LPSTR ModuleName,
                          IN LPSTR FileName,
                          IN DWORD dwLineNumber,
                          OUT PLONG plDisplacement,
                          IN OUT PIMAGEHLP_LINE Line)
  {
#ifdef SYMENG_EXTRAWORK
    if (nullptr == m_pfnSymGetLineFromName) return FALSE;
    return (*m_pfnSymGetLineFromName)(m_hProcess, ModuleName, FileName, dwLineNumber, plDisplacement, Line);
#else
    return ::SymGetLineFromName(m_hProcess, ModuleName, FileName, dwLineNumber, plDisplacement, Line);
#endif  // SYMENG_EXTRAWORK
  }

  BOOL SymGetLineNext(IN OUT PIMAGEHLP_LINE Line)
  {
#ifdef SYMENG_EXTRAWORK
    if (nullptr == m_pfnSymGetLineNext) return FALSE;
    return (*m_pfnSymGetLineNext)(m_hProcess, Line);
#else
    return ::SymGetLineNext(m_hProcess, Line);
#endif  // SYMENG_EXTRAWORK
  }

  BOOL SymGetLinePrev(IN OUT PIMAGEHLP_LINE Line)
  {
#ifdef SYMENG_EXTRAWORK
    if (nullptr == m_pfnSymGetLinePrev) return FALSE;
    return (*m_pfnSymGetLinePrev)(m_hProcess, Line);
#else
    return ::SymGetLinePrev(m_hProcess, Line);
#endif  // SYMENG_EXTRAWORK
  }

  // What is this?
  BOOL SymMatchFileName(IN LPSTR FileName,
                        IN LPSTR Match,
                        OUT LPSTR *FileNameStop,
                        OUT LPSTR *MatchStop)
  {
#ifdef SYMENG_EXTRAWORK
    if (nullptr == m_pfnSymMatchFileName) return FALSE;
    return (*m_pfnSymMatchFileName)(FileName, Match, FileNameStop, MatchStop);
#else
    return ::SymMatchFileName(FileName, Match, FileNameStop, MatchStop);
#endif  // SYMENG_EXTRAWORK
  }

  /* ----------------------------------------------------------------------
                           */

  LPVOID SymFunctionTableAccess(DWORD AddrBase) { return ::SymFunctionTableAccess(m_hProcess, AddrBase); }

  BOOL SymGetSearchPath(OUT LPSTR SearchPath,
                        IN DWORD SearchPathLength) { return ::SymGetSearchPath(m_hProcess, SearchPath, SearchPathLength); }

  BOOL SymSetSearchPath(IN LPSTR SearchPath) { return ::SymSetSearchPath(m_hProcess, SearchPath); }

  BOOL SymRegisterCallback(IN PSYMBOL_REGISTERED_CALLBACK CallbackFunction,
                           IN PVOID UserContext) { return ::SymRegisterCallback(m_hProcess, CallbackFunction, UserContext); }


  /* ----------------------------------------------------------------------
                          */
protected:
  // The unique value that will be used for this instance of the
  // symbol engine.  
  // process value, just a unique value.
  HANDLE m_hProcess;

#ifdef SYMENG_EXTRAWORK
  // The function pointers to the new symbol engine source and line
  // functions.
  PFNSYMGETLINEFROMADDR m_pfnSymGetLineFromAddr;
  PFNSYMGETLINEFROMNAME m_pfnSymGetLineFromName;
  PFNSYMGETLINENEXT m_pfnSymGetLineNext;
  PFNSYMGETLINEPREV m_pfnSymGetLinePrev;
  PFNSYMMATCHFILENAME m_pfnSymMatchFileName;
#endif // SYMENG_EXTRAWORK

};

// //////////////////////////////////////////////////////////// 
#endif // _SYMBOLENGINE_H