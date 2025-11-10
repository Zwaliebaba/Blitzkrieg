#ifndef __COMMONFILESYSTEM_H__
#define __COMMONFILESYSTEM_H__

#pragma once

struct SCommonFileInfo
{
  DWORD dwModTime;// file modification time
  IDataStorage *pStorage;// storage, this file stored in
  //
  SCommonFileInfo() : dwModTime(0), pStorage(nullptr) {}
  SCommonFileInfo(DWORD _dwModTime, IDataStorage *_pStorage) : dwModTime(_dwModTime), pStorage(_pStorage) {}
};

class CCommonFileSystemEnumerator : public IStorageEnumerator
{
  OBJECT_MINIMAL_METHODS(CCommonFileSystemEnumerator);
  std::string szMask;// enumeration mask
  std::string szFileName;// current enumerated file name
  SStorageElementStats stats;// temporary data storage to fill each call
  //
  using CFilesMap = std::hash_map<std::string, SCommonFileInfo>;
  const CFilesMap &files;
  CFilesMap::const_iterator itCurrFile;
  bool bReset;

public:
  CCommonFileSystemEnumerator(const CFilesMap &_files) : files(_files), itCurrFile(files.begin()), bReset(true) {}
  //
  void STDCALL Reset(const char *pszName) override;
  bool STDCALL Next() override;
  const SStorageElementStats * STDCALL GetStats() const override { return &stats; }
};

class CCommonFileSystem : public IDataStorage
{
  OBJECT_MINIMAL_METHODS(CCommonFileSystem);
  //
  CPtr<IDataStorage> pZipStorage;// zip file system
  CPtr<IDataStorage> pFileStorage;// open file system
  using CFilesMap = std::hash_map<std::string, SCommonFileInfo>;
  CFilesMap files;// correspondence between the file name and the information necessary to obtain it
  //
  std::string szBase;
  DWORD dwStorageAccessMode;
  //
  void EnumerateFiles(const std::string &szName, IDataStorage *pStorage);

public:
  CCommonFileSystem(const char *pszName, DWORD dwAccessMode);
  // check if such thread exists
  const bool STDCALL IsStreamExist(const char *pszName) override;
  // create and open a stream with the specified name and access rights
  IDataStream * STDCALL CreateStream(const char *pszName, DWORD dwAccessMode) override;
  // open an existing stream with the specified name and permissions
  IDataStream * STDCALL OpenStream(const char *pszName, DWORD dwAccessMode) override;
  // get stream description
  bool STDCALL GetStreamStats(const char *pszName, SStorageElementStats *pStats) override;
  // kill storage element
  bool STDCALL DestroyElement(const char *pszName) override;
  // rename element
  bool STDCALL RenameElement(const char *pszOldName, const char *pszNewName) override;
  // enumeration of elements
  IStorageEnumerator * STDCALL CreateEnumerator() override;
  // get the name of this storage
  const char * STDCALL GetName() const override { return szBase.c_str(); }
  // add new MOD
  bool STDCALL AddStorage(IDataStorage *pStorage, const char *pszName) override;
  // remove MOD
  bool STDCALL RemoveStorage(const char *pszName) override;
};

#endif // __COMMONFILESYSTEM_H__