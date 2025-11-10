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

  void STDCALL Reset(const char *pszMask) override;
  bool STDCALL Next() override;
  const SStorageElementStats * STDCALL GetStats() const override { return &stats; }
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
  using CFilesMap = std::hash_map<std::string, SZipArchiveFileInfo>;
  CFilesMap files;// correspondence between the file name and the information necessary to obtain it
  //
  std::string szBase;
  DWORD dwStorageAccessMode;

public:
  CZipFileSystem(const char *pszName, DWORD dwAccessMode);
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
  // external service
  bool AddZipFile(IDataStream *pStream, const std::string &szZipFileName);
  // add new MOD
  bool STDCALL AddStorage(IDataStorage *pStorage, const char *pszName) override;
  // remove MOD
  bool STDCALL RemoveStorage(const char *pszName) override;
};

#endif // __ZIPFILESYSTEM_H__