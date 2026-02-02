#ifndef __MEMFILESYSTEM_H__
#define __MEMFILESYSTEM_H__

#pragma once

class CMemFileStream : public IDataStream
{
  OBJECT_NORMAL_METHODS(CMemFileStream);
  DECLARE_SERIALIZE;
  //
  CPtr<IDataStorage> pStorage;// parent storage
  std::vector<BYTE> data;// memory data of this stream
  int nBeginPos;// locked begin position
  int nCurrPos;// current stream position
  // file stats
  std::string szName;
  SStorageElementStats stats;
  //
  void ResizeToFit(int nSize)
  {
    if (nSize > data.size())
    {
      data.reserve(static_cast<int>(nSize * 1.3));
      data.resize(nSize);
    }
  }

public:
  CMemFileStream() : nBeginPos(0), nCurrPos(0) { data.reserve(1024); }
  CMemFileStream(const CMemFileStream &stream) : data(stream.data), nBeginPos(0), nCurrPos(0) {}
  CMemFileStream(BYTE *pData, int nLength) : data(pData, pData + nLength), nBeginPos(0), nCurrPos(0) {}
  explicit CMemFileStream(int nSize, IDataStorage *_pStorage) : pStorage(_pStorage), data(nSize), nBeginPos(0), nCurrPos(0) {}
  //
  void *GetBuffer() { return &(data[0]); }

  void SetStats(const SStorageElementStats &_stats)
  {
    stats = _stats;
    szName = _stats.pszName;
    stats.pszName = szName.c_str();
  }

  //
  // read/write data
  int Read(void *pBuffer, int nLength) override;
  int Write(const void *pBuffer, int nLength) override;
  // declare the current position in the stream as the beginning of the stream
  int LockBegin() override;
  // return the start of the stream to the zero position
  int UnlockBegin() override;
  // current position in the stream
  int GetPos() const override;
  // set the current position in the stream
  int Seek(int offset, STREAM_SEEK from) override;
  // get stream size
  int GetSize() const override;
  // change stream size
  bool SetSize(int nSize) override;
  // copy 'nLength' byte from current stream position to current 'pDstStream' stream position
  int CopyTo(IDataStream *pDstStream, int nLength) override;
  // reset all cached data
  void Flush() override;
  // get information about the stream
  void GetStats(SStorageElementStats *pStats) override;
};

class CMemFileSystem : public IDataStorage
{
  OBJECT_MINIMAL_METHODS(CMemFileSystem);
  std::string szBase;
  DWORD dwStorageAccessMode;
  using CStreamsMap = std::unordered_map<std::string, CPtr<IDataStream>>;
  CStreamsMap streams;

public:
  CMemFileSystem(DWORD dwAccessMode);
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

#endif // __MEMFILESYSTEM_H__