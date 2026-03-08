#ifndef __STREAMIO_H__
#define __STREAMIO_H__

#pragma once

enum EStorageElementType
{
  SET_STORAGE = 1,
  SET_STREAM = 2,
  SET_FORCE_DWORD = 0x7fffffff
};

enum EStreamAccessMode
{
  STREAM_ACCESS_READ = 0x00000001,
  STREAM_ACCESS_WRITE = 0x00000002,
  STREAM_ACCESS_APPEND = 0x00000004,
  STREAM_ACCESS_TEXT = 0x00000008,
  STREAM_ACCESS_FORCE_DWORD = 0x7fffffff
};

enum EStorageType
{
  STORAGE_TYPE_MOD = 0,
  STORAGE_TYPE_COMMON = 1,
  STORAGE_TYPE_FILE = 2,
  STORAGE_TYPE_ZIP = 3,
  STORAGE_TYPE_MEM = 4,
  STORAGE_TYPE_FORCE_DWORD = 0x7fffffff
};

// special structure, which's bit fields represent Win32 date/time format in the correct form
struct SWin32Time
{
  union
  {
    struct
    {
      DWORD seconds : 5;// seconds (0..29 with 2 sec. interval)
      DWORD minutes : 6;// minutes (0..59)
      DWORD hours : 5;// hours (0..23)
      DWORD day : 5;// day (1..31)
      DWORD month : 4;// month(1..12)
      DWORD year : 7;// year (0..119 relative to 1980)
    };

    struct
    {
      WORD wTime;
      WORD wDate;
    };

    DWORD dwFulltime;
  };

  //
  SWin32Time() {}
  SWin32Time(DWORD _dwFulltime) : dwFulltime(_dwFulltime) {}
  WORD GetDate() const { return wDate; }
  WORD GetTime() const { return wTime; }
  operator DWORD() const { return dwFulltime; }
};

struct SStorageElementStats
{
  const char *pszName;// element name
  EStorageElementType type;// type: storage/stream
  int nSize;// storage size
  SWin32Time ctime;// creation time
  SWin32Time mtime;// modification time
  SWin32Time atime;// last access time
};

interface IStorageEnumerator : IRefCount
{
  virtual void STDCALL Reset(const char *pszName) = 0;
  virtual bool STDCALL Next() = 0;
  virtual const SStorageElementStats * STDCALL GetStats() const = 0;
};

interface IDataStream : IRefCount
{
  // read/write data
  virtual int STDCALL Read(void *pBuffer, int nLength) = 0;
  virtual int STDCALL Write(const void *pBuffer, int nLength) = 0;
  // declare the current position in the stream as the beginning of the stream
  virtual int STDCALL LockBegin() = 0;
  // return the start of the stream to the zero position
  virtual int STDCALL UnlockBegin() = 0;
  // current position in the stream
  virtual int STDCALL GetPos() const = 0;
  // set the current position in the stream
  virtual int STDCALL Seek(int offset, STREAM_SEEK from) = 0;
  // get stream size
  virtual int STDCALL GetSize() const = 0;
  // change stream size
  virtual bool STDCALL SetSize(int nSize) = 0;
  // copy 'nLength' byte from current stream position to current 'pDstStream' stream position
  virtual int STDCALL CopyTo(IDataStream *pDstStream, int nLength) = 0;
  // reset all cached data
  virtual void STDCALL Flush() = 0;
  // get information about the stream
  virtual void STDCALL GetStats(SStorageElementStats *pStats) = 0;
};

interface IDataStorage : IRefCount
{
  // type ID
  enum { tidTypeID = 0 };

  // check if such thread exists
  virtual const bool STDCALL IsStreamExist(const char *pszName) = 0;
  // create and open a stream with the specified name and access rights
  virtual IDataStream * STDCALL CreateStream(const char *pszName, DWORD dwAccessMode) = 0;
  // open an existing stream with the specified name and permissions
  virtual IDataStream * STDCALL OpenStream(const char *pszName, DWORD dwAccessMode) = 0;
  // get stream description
  virtual bool STDCALL GetStreamStats(const char *pszName, SStorageElementStats *pStats) = 0;
  // kill storage element
  virtual bool STDCALL DestroyElement(const char *pszName) = 0;
  // rename element
  virtual bool STDCALL RenameElement(const char *pszOldName, const char *pszNewName) = 0;
  // enumeration of elements
  virtual IStorageEnumerator * STDCALL CreateEnumerator() = 0;
  // get the name of this storage
  virtual const char * STDCALL GetName() const = 0;
  // add new MOD by name
  virtual bool STDCALL AddStorage(IDataStorage *pStorage, const char *pszName) = 0;
  // remove MOD by name
  virtual bool STDCALL RemoveStorage(const char *pszName) = 0;
};

inline std::string GetStreamName(IDataStream *pStream)
{
  SStorageElementStats stats;
  pStream->GetStats(&stats);
  return stats.pszName == nullptr ? "" : stats.pszName;
}

#endif // __STREAMIO_H__