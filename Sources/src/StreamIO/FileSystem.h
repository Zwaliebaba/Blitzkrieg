#ifndef __FILESYSTEM_H__
#define __FILESYSTEM_H__

#pragma once

class CFileSystemEnumerator : public IStorageEnumerator
{
  OBJECT_MINIMAL_METHODS(CFileSystemEnumerator);
  //
  HANDLE hFind;// handle to FindFile
  WIN32_FIND_DATA findinfo;// result of the FileFind
  std::string szPath;// path to the file
  std::string szMask;// enumeration mask
  std::string szBase;// base dir for enumeration (from the storage)
  std::string szFoundFileName;// filename
  SStorageElementStats stats;// temporary data storage to fill each call
  //
  const bool IsFindValid() const { return hFind != INVALID_HANDLE_VALUE; }
  bool FindFirstFile();
  bool FindNextFile();
  void Close();
  void FillStats();
  //
  // current file attributes check
  bool IsReadOnly() const { return (findinfo.dwFileAttributes & FILE_ATTRIBUTE_READONLY) != 0; }
  bool IsSystem() const { return (findinfo.dwFileAttributes & FILE_ATTRIBUTE_SYSTEM) != 0; }
  bool IsHidden() const { return (findinfo.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN) != 0; }
  bool IsTemporary() const { return (findinfo.dwFileAttributes & FILE_ATTRIBUTE_TEMPORARY) != 0; }
  bool IsNormal() const { return (findinfo.dwFileAttributes & FILE_ATTRIBUTE_NORMAL) != 0; }
  bool IsArchive() const { return (findinfo.dwFileAttributes & FILE_ATTRIBUTE_ARCHIVE) != 0; }
  bool IsCompressed() const { return (findinfo.dwFileAttributes & FILE_ATTRIBUTE_COMPRESSED) != 0; }
  bool IsDirectory() const { return (findinfo.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0; }
  // special kind of directory: '.' 
  bool IsDots() const
  {
    return IsDirectory() &&
           (((findinfo.cFileName[0] == '.')) &&
            ((findinfo.cFileName[1] == '\0') || ((findinfo.cFileName[1] == '.') && (findinfo.cFileName[2] == '\0'))));
  }

public:
  CFileSystemEnumerator(const std::string &_szBase)
    : hFind(INVALID_HANDLE_VALUE), szBase(_szBase) { Zero(stats); }

  virtual ~CFileSystemEnumerator();
  //
  void STDCALL Reset(const char *pszMask) override;
  bool STDCALL Next() override;
  const SStorageElementStats * STDCALL GetStats() const override { return &stats; }
};

class CFileStream : public IDataStream
{
  OBJECT_MINIMAL_METHODS(CFileStream);
  //
  HANDLE hFile;// file, associated with this stream
  int nStreamBegin;// stream begin position
  DWORD dwAccessMode;// access mode
  // file stats
  std::string szName;
  SStorageElementStats stats;
  bool CanRead() const { return IsOpen() && (dwAccessMode & STREAM_ACCESS_READ); }
  bool CanWrite() const { return IsOpen() && (dwAccessMode & STREAM_ACCESS_WRITE); }

public:
  CFileStream(const char *pszFileName, DWORD dwAccessMode);
  virtual ~CFileStream();
  // read/write data
  int STDCALL Read(void *pBuffer, int nLength) override;
  int STDCALL Write(const void *pBuffer, int nLength) override;
  // declare the current position in the stream as the beginning of the stream
  int STDCALL LockBegin() override;
  // return the start of the stream to the zero position
  int STDCALL UnlockBegin() override;
  // current position in the stream
  int STDCALL GetPos() const override;
  // set the current position in the stream
  int STDCALL Seek(int offset, STREAM_SEEK from) override;
  // get stream size
  int STDCALL GetSize() const override;
  // change stream size
  bool STDCALL SetSize(int nSize) override;
  // copy 'nLength' byte from current stream position to current 'pDstStream' stream position
  int STDCALL CopyTo(IDataStream *pDstStream, int nLength) override;
  // reset all cached data
  void STDCALL Flush() override;
  // get information about the stream
  void STDCALL GetStats(SStorageElementStats *pStats) override;
  //
  bool IsOpen() const { return hFile != INVALID_HANDLE_VALUE; }
};

class CFileSystem : public IDataStorage
{
  OBJECT_MINIMAL_METHODS(CFileSystem);
  std::string szBase;
  DWORD dwStorageAccessMode;
  //
  bool CreatePathRecursive(const std::string &szName);

public:
  CFileSystem(const char *pszName, DWORD dwAccessMode, bool bCreate);
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

#endif // __FILESYSTEM_H__