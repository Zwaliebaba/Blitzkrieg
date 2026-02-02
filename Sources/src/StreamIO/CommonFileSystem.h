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
  using CFilesMap = std::unordered_map<std::string, SCommonFileInfo>;
  const CFilesMap &files;
  CFilesMap::const_iterator itCurrFile;
  bool bReset;

public:
  CCommonFileSystemEnumerator(const CFilesMap &_files) : files(_files), itCurrFile(files.begin()), bReset(true) {}
  //
  void Reset(const char *pszName) override;
  bool Next() override;
  const SStorageElementStats * GetStats() const override { return &stats; }
};

class CCommonFileSystem : public IDataStorage
{
  OBJECT_MINIMAL_METHODS(CCommonFileSystem);
  //
  CPtr<IDataStorage> pZipStorage;// zip file system
  CPtr<IDataStorage> pFileStorage;// open file system
  using CFilesMap = std::unordered_map<std::string, SCommonFileInfo>;
  CFilesMap files;// correspondence between the file name and the information necessary to obtain it
  //
  std::string szBase;
  DWORD dwStorageAccessMode;
  //
  void EnumerateFiles(const std::string &szName, IDataStorage *pStorage);

public:
  CCommonFileSystem(const char *pszName, DWORD dwAccessMode);
  // check if such thread exists
  const bool IsStreamExist(const char *pszName) override;
  // create and open a stream with the specified name and access rights
  IDataStream * CreateStream(const char *pszName, DWORD dwAccessMode) override;
  // open an existing stream with the specified name and permissions
  IDataStream * OpenStream(const char *pszName, DWORD dwAccessMode) override;
  // get stream description
  bool GetStreamStats(const char *pszName, SStorageElementStats *pStats) override;
  // kill storage element
  bool DestroyElement(const char *pszName) override;
  // rename element
  bool RenameElement(const char *pszOldName, const char *pszNewName) override;
  // enumeration of elements
  IStorageEnumerator * CreateEnumerator() override;
  // get the name of this storage
  const char * GetName() const override { return szBase.c_str(); }
  // add new MOD
  bool AddStorage(IDataStorage *pStorage, const char *pszName) override;
  // remove MOD
  bool RemoveStorage(const char *pszName) override;
};
