#include "StdAfx.h"

#include "CommonFileSystem.h"

// ************************************************************************************************************************ //
// **
// ** common file system enumerator
// **
// **
// **
// ************************************************************************************************************************ //

void CCommonFileSystemEnumerator::Reset(const char *pszName)
{
  szMask = pszName;
  szFileName.clear();
  Zero(stats);
  bReset = true;
  itCurrFile = files.end();
}

bool CCommonFileSystemEnumerator::Next()
{
  // initialize after reset
  if (bReset)
  {
    itCurrFile = files.begin();
    bReset = false;
    // check for correcntess
    if (itCurrFile == files.end()) return false;
  }
  // increment - to next file
  ++itCurrFile;
  if (itCurrFile == files.end()) return false;
  //
  szFileName = itCurrFile->first;
  stats.pszName = szFileName.c_str();
  stats.mtime = stats.atime = stats.ctime = itCurrFile->second.dwModTime;
  stats.type = SET_STREAM;
  stats.nSize = -1;

  return true;
}

// ************************************************************************************************************************ //
// **
// ** common file system
// **
// **
// **
// ************************************************************************************************************************ //

CCommonFileSystem::CCommonFileSystem(const char *pszName, DWORD dwAccessMode)
  : dwStorageAccessMode(dwAccessMode)
{
  NI_ASSERT_SLOW_T((dwAccessMode & (STREAM_ACCESS_WRITE | STREAM_ACCESS_APPEND)) == 0, "Can't write to common file system - still not realized for all components");
  dwStorageAccessMode &= ~(STREAM_ACCESS_WRITE | STREAM_ACCESS_APPEND);
  // extact file mask to enumerate
  std::string szName = pszName;
  std::string szMask;
  int nPos = szName.rfind('\\');
  if (nPos == std::string::npos)
  {
    szMask = szName.empty() ? "*.zip" : szName;
    szName = ".\\";
  }
  else
  {
    szMask = nPos + 1 == szName.size() ? "*.zip" : szName.substr(nPos + 1);
    szName = szName.substr(0, nPos + 1);
  }
  // enumerate files from zip file system
  pZipStorage = OpenStorage(pszName, dwStorageAccessMode, STORAGE_TYPE_ZIP);
  EnumerateFiles("", pZipStorage);
  // enumerate files from open file system
  pFileStorage = OpenStorage(szName.c_str(), dwStorageAccessMode, STORAGE_TYPE_FILE);
  EnumerateFiles("", pFileStorage);
  // base name
  szBase = pFileStorage->GetName();
}

void CCommonFileSystem::EnumerateFiles(const std::string &szName, IDataStorage *pStorage)
{
  CPtr<IStorageEnumerator> pEnum = pStorage->CreateEnumerator();
  for (pEnum->Reset((szName + "*.*").c_str()); pEnum->Next();)
  {
    const SStorageElementStats *pStats = pEnum->GetStats();
    if (pStats->type == SET_STORAGE) EnumerateFiles(szName + pStats->pszName + '\\', pStorage);
    else
    {
      std::string szFileName = szName + pStats->pszName;
      NStr::ToLower(szFileName);
      SCommonFileInfo &info = files[szFileName];
      if ((info.pStorage == nullptr) || (info.dwModTime < pStats->mtime))
      {
        info.dwModTime = pStats->mtime;
        info.pStorage = pStorage;
      }
    }
  }
}

// create and open a stream with the specified name and access rights
IDataStream *CCommonFileSystem::CreateStream(const char *pszName, DWORD dwAccessMode)
{
  NI_ASSERT_T(0, "Have no write access to common file system");
  return nullptr;
}

// open an existing stream with the specified name and permissions
IDataStream *CCommonFileSystem::OpenStream(const char *pszName, DWORD dwAccessMode)
{
  std::string szName = pszName;
  NStr::ToLower(szName);
  CFilesMap::iterator pos = files.find(szName);
  IDataStream *pRes = nullptr;
  if (pos != files.end())// open the file from the corresponding storage
    pRes = pos->second.pStorage->OpenStream(szName.c_str(), dwAccessMode);
  else// Let's try to open the file from the disk (if it appeared after initializing the common file system - for editors)
    pRes = pFileStorage->OpenStream(pszName, dwAccessMode);
#ifndef _DONT_USE_SINGLETON
  // CRAP{ to catch non-existent files
  if ((pRes == nullptr) && (GetGlobalVar("report", 0) == 1)) GetSingleton<IConsoleBuffer>()->WriteASCII(CONSOLE_STREAM_CONSOLE, NStr::Format("Can't open stream \"%s\" with access mode 0x%.8x", pszName, dwAccessMode), 0xffff0000, true);
  // CRAP}
#endif // _DONT_USE_SINGLETON
  return pRes;
}

bool CCommonFileSystem::GetStreamStats(const char *pszName, SStorageElementStats *pStats)
{
  if (!IsStreamExist(pszName)) return false;
  std::string szName = pszName;
  NStr::ToLower(szName);
  CFilesMap::iterator pos = files.find(szName);
  if (pos != files.end())// file from its corresponding storage
    return pos->second.pStorage->GetStreamStats(pszName, pStats);
  // let's try the file from the disk (if it appeared after initializing the common file system - for editors)
  return pFileStorage->GetStreamStats(pszName, pStats);
}

// kill storage element
bool CCommonFileSystem::DestroyElement(const char *pszName)
{
  NI_ASSERT_T(0, "Have no write access to common file system");
  return false;
}

// rename element
bool CCommonFileSystem::RenameElement(const char *pszOldName, const char *pszNewName)
{
  NI_ASSERT_T(0, "Have no write access to common file system");
  return false;
}

// enumeration of elements
IStorageEnumerator *CCommonFileSystem::CreateEnumerator() { return new CCommonFileSystemEnumerator(files); }

// add new MOD
bool CCommonFileSystem::AddStorage(IDataStorage *pStorage, const char *pszName)
{
  NI_ASSERT_T(0, "Can't add new storage to the common file system");
  return false;
}

// remove MOD
bool CCommonFileSystem::RemoveStorage(const char *pszName)
{
  NI_ASSERT_T(0, "Can't remove storage from common file system");
  return false;
}

const bool CCommonFileSystem::IsStreamExist(const char *pszName)
{
  std::string szName = pszName;
  NStr::ToLower(szName);
  return (files.find(szName) != files.end()) || pFileStorage->IsStreamExist(szName.c_str());
}