#ifndef __MODFILESYSTEM_H__
#define __MODFILESYSTEM_H__

#pragma once

class CModFileSystemEnumerator : public IStorageEnumerator
{
  OBJECT_MINIMAL_METHODS(CModFileSystemEnumerator);
  SStorageElementStats stats;// temporary data storage to fill each call
  using CStatsMap = std::map<std::string, SStorageElementStats>;
  CStatsMap files;// all file stats
  CStatsMap::const_iterator itCurrFile;// current iterated file
  bool bReset;

public:
  CModFileSystemEnumerator() {}
  //
  void AddFile(const std::string &szName, const SStorageElementStats &stats) { files[szName] = stats; }
  void STDCALL Reset(const char *pszName) override;
  bool STDCALL Next() override;
  const SStorageElementStats * STDCALL GetStats() const override { return &stats; }
};

class CModFileSystem : public IDataStorage
{
  OBJECT_MINIMAL_METHODS(CModFileSystem);
  //
  using SFileSystemDesc = std::pair<std::string, CPtr<IDataStorage>>;
  using CFileSystemsList = std::list<SFileSystemDesc>;
  CFileSystemsList filesystems;// available file systems, last file system - base
  DWORD dwStorageAccessMode;// access mode - READ-ONLY for this storage
public:
  CModFileSystem(const char *pszName, DWORD dwAccessMode);
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
  const char * STDCALL GetName() const override;
  // add new MOD
  bool STDCALL AddStorage(IDataStorage *pStorage, const char *pszName) override;
  // remove MOD
  bool STDCALL RemoveStorage(const char *pszName) override;
};

#endif // __MODFILESYSTEM_H__