#ifndef __ZIPFILESYSTEM_H__
#define __ZIPFILESYSTEM_H__

#pragma once

#include "ZipFile.h"

struct SZipFileDesc : CZipFile
{
  std::string szZipFileName;
};

using CZipFilesList = std::list<SZipFileDesc>;

class CZipFileSystemEnumerator : public IStorageEnumerator
{
  OBJECT_MINIMAL_METHODS(CZipFileSystemEnumerator);
  //
  CPtr<IDataStorage> pStorage;// parent storage
  const CZipFilesList &zipfiles;// list of open zip files
  CZipFilesList::const_iterator posZipFile;// current zip file
  int nFileInZip;// current file inside zip
  std::string szFileName;// current enumerated file name
  SStorageElementStats stats;// temporary data storage to fill each call
  //
  bool NextEntry();

public:
  CZipFileSystemEnumerator(const CZipFilesList &_zipfiles, IDataStorage *pStorage);

  void Reset(const char *pszMask) override;
  bool Next() override;
  const SStorageElementStats * GetStats() const override { return &stats; }
};

class CZipFileSystem : public IDataStorage
{
  OBJECT_MINIMAL_METHODS(CZipFileSystem);
  //
  struct SZipDirHeader;
  struct SZipDirFileHeader;
  struct SZipLocalHeader;

  struct SZipArchiveFileInfo
  {
    int nIndex;// index of this file in the zip
    int nSize;// size of this file
    SZipFileDesc *pZipFile;// zip-file, which contains this file
    //
    SZipArchiveFileInfo() : nIndex(-1), nSize(-1), pZipFile(nullptr) {}
  };

  //
  CZipFilesList zipfiles;// list of open zip files
  using CFilesMap = std::unordered_map<std::string, SZipArchiveFileInfo>;
  CFilesMap files;// correspondence between the file name and the information necessary to obtain it
  //
  std::string szBase;
  DWORD dwStorageAccessMode;

public:
  CZipFileSystem(const char *pszName, DWORD dwAccessMode);
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
  // external service
  bool AddZipFile(IDataStream *pStream, const std::string &szZipFileName);
  // add new MOD
  bool AddStorage(IDataStorage *pStorage, const char *pszName) override;
  // remove MOD
  bool RemoveStorage(const char *pszName) override;
};

#endif // __ZIPFILESYSTEM_H__