#ifndef __STREAMS_H__
#define __STREAMS_H__

#pragma once

// streams in memory for organizing fast work with data from virtual functions
// only for working inside the module!!!
// mainly for use in structure saver

// this classes use big endian numbers format

// redundant flow interface for fast operations
// binary mode only

class CDataStream
{
protected:
  enum EFlags
  {
    F_Wasted = 1,
    F_CanRead = 2,
    F_CanWrite = 4,
    F_Failed = 8,
  };

  unsigned char *pBuffer, *pCurrent, *pReservedEnd;
  mutable unsigned char *pFileEnd;
  unsigned int nBufferStart;
  int nFlags;

  int CanRead() { return nFlags & F_CanRead; }
  int CanWrite() { return nFlags & F_CanWrite; }
  int IsWasted() { return nFlags & F_Wasted; }
  void SetWasted() { nFlags |= F_Wasted; }
  void SetFailed() { nFlags |= F_Failed; }
  void ClearWasted() { nFlags &= ~F_Wasted; }
  void FixupSize() const { if (pCurrent > pFileEnd) pFileEnd = pCurrent; }// this limits us to max file size 2M
  // possible exceptional cases during the operation of the object
  // a) insufficient buffer size for direct access operation
  virtual void AllocForDirectReadAccess(unsigned int nSize) = 0;
  // must store the current buffer contents in memory, nSize is the required buffer size
  virtual void AllocForDirectWriteAccess(unsigned int nSize) = 0;
  // b) message about end of direct access mode
  virtual void NotifyFinishDirectAccess() {}
  // c) reading/writing that does not fit into the current buffer
  virtual unsigned int DirectRead(void *pDest, unsigned int nSize) = 0;
  virtual unsigned int DirectWrite(const void *pSrc, unsigned int nSize) = 0;
  //
  // functions to provide direct (and therefore fast) access to data
  // can be called multiple times
  // reserve nSize bytes for reading/writing (this amount of information
  // must be accessible by the returned pointer after calling this function)
  // the function should always succeed
  inline unsigned char *ReserveR(unsigned int nSize);
  // the ReserveW function must leave the entire current buffer in memory
  inline unsigned char *ReserveW(unsigned int nSize);
  // fixes up stream size if needed
  // end direct data access mode, the function must rearrange the pointer
  // current position in pFinish
  void Free(unsigned char *pFinish)
  {
    pCurrent = pFinish;
    NotifyFinishDirectAccess();
  }

  // non-checking read and write
  void RRead(void *pDest, unsigned int nSize)
  {
    memcpy(pDest, pCurrent, nSize);
    pCurrent += nSize;
    ASSERT(pCurrent <= pReservedEnd);
  }

  void RWrite(const void *pSrc, unsigned int nSize)
  {
    memcpy(pCurrent, pSrc, nSize);
    pCurrent += nSize;
    ASSERT(pCurrent <= pReservedEnd);
  }

  // exceptional case
  unsigned int ReadOverflow(void *pDest, unsigned int nSize);
  //
public:
  CDataStream() { nBufferStart = 0; }
  virtual ~CDataStream() {}
  // positioning
  virtual void Seek(unsigned int nPos) = 0;
  // void Trunc(); 
  // regular functions for reading/writing from/to a stream
  inline unsigned int Read(void *pDest, unsigned int nSize);
  inline unsigned int Write(const void *pSrc, unsigned int nSize);
  //
  int GetSize() const
  {
    FixupSize();
    return pFileEnd - pBuffer + nBufferStart;
  }

  int GetPosition() const { return pCurrent - pBuffer + nBufferStart; }
  bool IsOk() const { return (nFlags & F_Failed) == 0; }
  void SetOk() { nFlags &= ~F_Failed; }
  //
  // standard I/O operations
  bool ReadString(std::string &res, int nMaxSize = -1);
  void WriteString(const std::string &res);

  template<class T>
  CDataStream &operator>>(T &res)
  {
    Read(&res, sizeof(res));
    return *this;
  }

  template<class T>
  CDataStream &operator<<(const T &res)
  {
    Write(&res, sizeof(res));
    return *this;
  }

  template<>
  CDataStream &operator>>(std::string &res)
  {
    ReadString(res);
    return *this;
  }

  template<>
  CDataStream &operator<<(const std::string &res)
  {
    WriteString(res);
    return *this;
  }

  // operations with whole streams
  inline unsigned int ReadTo(CDataStream &dst, unsigned int nSize);
  inline unsigned int WriteFrom(CDataStream &src);

  friend class CBitLocker;
};

// reading fixed memory, useful when working with memory mapped files
class CFixedMemStream : public CDataStream
{
protected:
  void AllocForDirectReadAccess(unsigned int nSize) override;
  // write is forbidden
  void AllocForDirectWriteAccess(unsigned int nSize) override;
  // fill with zero exceeding request
  unsigned int DirectRead(void *pDest, unsigned int nSize) override;
  unsigned int DirectWrite(const void *pSrc, unsigned int nSize) override;

public:
  CFixedMemStream(const void *pCData, int nSize)
  {
    auto pData = const_cast<void *>(pCData);
    pBuffer = static_cast<unsigned char *>(pData);
    pCurrent = pBuffer;
    pReservedEnd = pBuffer + nSize;
    pFileEnd = pReservedEnd;
    nFlags = F_CanRead;
  }

  void Seek(unsigned int nPos) override
  {
    pCurrent = pBuffer + nPos;
    ASSERT(pCurrent <= pReservedEnd);
  }
};

// stream located entirely in memory
class CMemoryStream : public CDataStream
{
protected:
  // resize so that at least nNewSize bytes are available
  void FixupBufferSize(int nNewSize);
  void AllocForDirectReadAccess(unsigned int nSize) override;
  void AllocForDirectWriteAccess(unsigned int nSize) override;
  unsigned int DirectRead(void *pDest, unsigned int nSize) override;
  unsigned int DirectWrite(const void *pSrc, unsigned int nSize) override;
  void CopyMemoryStream(const CMemoryStream &src);

public:
  CMemoryStream();
  ~CMemoryStream() override;
  CMemoryStream(const CMemoryStream &src);
  CMemoryStream &operator=(const CMemoryStream &src);
  void SetRMode() { nFlags = (nFlags & ~(F_CanRead | F_CanWrite)) | F_CanRead; }
  void SetWMode() { nFlags = (nFlags & ~(F_CanRead | F_CanWrite)) | F_CanWrite; }
  void SetRWMode() { nFlags = nFlags | (F_CanRead | F_CanWrite); }

  void Seek(unsigned int nPos) override
  {
    FixupSize();
    pCurrent = pBuffer + nPos;
    if (pCurrent > pReservedEnd) FixupBufferSize(pCurrent - pBuffer);
  }

  // special memory stream funcs, this functions work only for memory stream
  void Clear()
  {
    pFileEnd = pBuffer;
    pCurrent = pBuffer;
    nFlags = F_CanRead | F_CanWrite;
  }

  // fast buffer access, use only if performance is of paramount importance
  const unsigned char *GetBuffer() const { return pBuffer; }
  unsigned char *GetBufferForWrite() const { return pBuffer; }

  void SetSize(int nSize)
  {
    pFileEnd = pBuffer + nSize;
    pCurrent = pBuffer;
    if (pFileEnd > pReservedEnd) FixupBufferSize(nSize);
  }

  void SetSizeDiscard(int nSize);
};

// universal buffering stream
class CBufferedStream : public CDataStream
{
  void FlushBuffer();
  void SetNewBufferSize(unsigned int nSize);
  void ShiftBuffer();
  void LoadBufferForced(int nPos);// set pCurrent to nPos and load buffer from nPos
  //
  void AllocForDirectReadAccess(unsigned int nSize) override;
  void AllocForDirectWriteAccess(unsigned int nSize) override;
  void NotifyFinishDirectAccess() override;
  // c) reading/writing that does not fit into the current buffer
  unsigned int DirectRead(void *pDest, unsigned int nSize) override;
  unsigned int DirectWrite(const void *pSrc, unsigned int nSize) override;
  CBufferedStream(const CBufferedStream &a) { ASSERT(0); }

  CBufferedStream &operator=(const CBufferedStream &a)
  {
    ASSERT(0);
    return *this;
  }

protected:
  void StartAccess(unsigned int nFileSize, unsigned int nSize);
  void FinishAccess();
  //
  virtual unsigned int DoRead(unsigned int nPos, void *pDest, unsigned int nSize) = 0;
  virtual unsigned int DoWrite(unsigned int nPos, const void *pSrc, unsigned int nSize) = 0;

public:
  CBufferedStream()
  {
    pBuffer = nullptr;
    nFlags = 0;
  }

  ~CBufferedStream() override { FinishAccess(); }
  void Seek(unsigned int nPos) override;
};

// class for sequential writing/reading of data, including write capability
// or reading bit data, can be used on arbitrary areas
// memory
class CBitStream
{
public:
  enum Mode
  {
    read,
    write
  };

protected:
  unsigned char *pCurrent;
  unsigned char *pBitPtr;// for bit writing
  unsigned int nBits;
  unsigned char nBitsCount;// bits and bit counter
  static unsigned int nBitsMask[32];

#ifdef _DEBUG
  Mode mode;
  unsigned char *pReservedEnd;

  void CheckCurrentR()
  {
    ASSERT(pCurrent <= pReservedEnd);
    ASSERT(mode == read);
  }

  void CheckCurrentW()
  {
    ASSERT(pCurrent <= pReservedEnd);
    ASSERT(mode == write);
  }
#else
  void CheckCurrentR() {}
  void CheckCurrentW() {}
#endif

  inline void Init(unsigned char *pData, Mode _mode, int nSize);

public:
  CBitStream(void *pData, Mode _mode, int nSize) { Init(static_cast<unsigned char *>(pData), _mode, nSize); }
  // result of read/write beyond data range is not determined
  void Read(void *pDest, unsigned int nSize)
  {
    memcpy(pDest, pCurrent, nSize);
    pCurrent += nSize;
    CheckCurrentR();
  }

  void Write(const void *pSrc, unsigned int nSize)
  {
    memcpy(pCurrent, pSrc, nSize);
    pCurrent += nSize;
    CheckCurrentW();
  }

  void ReadCString(std::string &res)
  {
    int nLeng = strlen((char *) pCurrent);
    res.assign((char *) pCurrent, nLeng);
    pCurrent += nLeng + 1;
    CheckCurrentR();
  }

  void WriteCString(const char *pSrc)
  {
    int nLeng = strlen(pSrc);
    memcpy(pCurrent, pSrc, nLeng + 1);
    pCurrent += nLeng + 1;
    CheckCurrentW();
  }

  void FlushBits()
  {
    if (nBitsCount)
    {
      nBitsCount = 0;
      if (pBitPtr) pBitPtr[0] = static_cast<char>(nBits);
    }
  }

  // not more then 24 bits per call
  inline void WriteBits(unsigned int _nBits, unsigned int _nBitsCount);
  inline void WriteBit(unsigned int _nBits);
  inline unsigned int ReadBits(unsigned int _nBitsCount);
  inline unsigned int ReadBit();
  // even more direct access, try to not use it, read only
  const unsigned char *GetCurrentPtr() const { return pCurrent; }
  // get pointer to place to write to later (not later then this object will be destructed)
  unsigned char *WriteDelayed(int nSize)
  {
    unsigned char *pRes = pCurrent;
    pCurrent += nSize;
    CheckCurrentW();
    return pRes;
  }

  //
  template<class T>
  void Write(const T &a) { Write(&a, sizeof(a)); }

  template<class T>
  void Read(T &a) { Read(&a, sizeof(a)); }

  template<>
  void Write<std::string>(const std::string &a) { WriteCString(a.c_str()); }

  template<>
  void Read<std::string>(std::string &a) { ReadCString(a); }

  //
  friend class CBitEmbedded;
};

// class for performing bitwise and high-speed I/O to a general purpose stream
// after CBitLocker starts working with CDataStream, direct operations with
// DataStream will lead to incorrect results
class CBitLocker : public CBitStream
{
  CDataStream *pData;
  unsigned char *pBuffer;

public:
  CBitLocker() : CBitStream(nullptr, read, 0) { pData = nullptr; }

  ~CBitLocker()
  {
    FlushBits();
    if (pData) pData->Free(pCurrent);
  }

  // once per life of this object
  void LockRead(CDataStream &data, unsigned int nSize);
  void LockWrite(CDataStream &data, unsigned int nSize);
  // alloc additional buffer space, for better performance minimize number of this
  // function calls
  void ReserveRead(unsigned int nSize);
  void ReserveWrite(unsigned int nSize);

  void Free()
  {
    ASSERT(pData);
    FlushBits();
    pData->Free(pCurrent);
    pData = nullptr;
  }
};

class CBitEmbedded : public CBitStream
{
  CBitStream &bits;

public:
  CBitEmbedded(CBitStream &_bits) :
#ifdef _DEBUG
    CBitStream(_bits.pCurrent, _bits.mode, _bits.pReservedEnd - _bits.pCurrent)
#else
    CBitStream(_bits.pCurrent, read, 0)
#endif
    , bits(_bits) {}

  ~CBitEmbedded() { bits.pCurrent = pCurrent; }
};


// implementation of inline functions for above classes

// CDataStream implementation

inline unsigned char *CDataStream::ReserveR(unsigned int nSize)
{
  ASSERT(CanRead());
  if (pCurrent + nSize > pReservedEnd) AllocForDirectReadAccess(nSize);
  return pCurrent;
}

// the ReserveW function must leave the entire current buffer in memory
inline unsigned char *CDataStream::ReserveW(unsigned int nSize)
{
  ASSERT(CanWrite());
  SetWasted();
  if (pCurrent + nSize > pReservedEnd) AllocForDirectWriteAccess(nSize);
  return pCurrent;
}

inline unsigned int CDataStream::Read(void *pDest, unsigned int nSize)
{
  ASSERT(CanRead());
  if (pCurrent + nSize <= pFileEnd)
  {
    if (pCurrent + nSize <= pReservedEnd)
    {
      {
        RRead(pDest, nSize);
        return nSize;
      }
    }
    return DirectRead(pDest, nSize);
  }
  return ReadOverflow(pDest, nSize);
}

inline unsigned int CDataStream::Write(const void *pSrc, unsigned int nSize)
{
  ASSERT(CanWrite());
  if (pCurrent + nSize <= pReservedEnd)
  {
    SetWasted();
    RWrite(pSrc, nSize);
    return nSize;
  }
  return DirectWrite(pSrc, nSize);
}

inline unsigned int CDataStream::ReadTo(CDataStream &dst, unsigned int nSize)
{
  dst.Seek(0);
  unsigned char *pBuf = dst.ReserveW(nSize);
  unsigned int nRes = Read(pBuf, nSize);
  dst.Free(pBuf + nSize);
  return nRes;
}

inline unsigned int CDataStream::WriteFrom(CDataStream &src)
{
  src.Seek(0);
  int nSize = src.GetSize();
  unsigned char *pBuf = src.ReserveR(nSize);
  unsigned int nRes = Write(pBuf, nSize);
  src.Free(pBuf + nSize);
  return nRes;
}

// CBitStream implementation

inline void CBitStream::Init(unsigned char *pData, Mode _mode, int nSize)
{
  pCurrent = pData;
  nBitsCount = 0;
  pBitPtr = nullptr;
#ifdef _DEBUG
  mode = _mode;
  pReservedEnd = pCurrent + nSize;
#endif
}

inline void CBitStream::WriteBits(unsigned int _nBits, unsigned int _nBitsCount)
{
  if (nBitsCount != 0)
  {
    nBits += (_nBits << nBitsCount);
    nBitsCount += _nBitsCount;
  }
  else
  {
    pBitPtr = pCurrent++;
    nBits = _nBits;
    nBitsCount = _nBitsCount;
  }
  while (nBitsCount > 8)
  {
    pBitPtr[0] = static_cast<unsigned char>(nBits);// ( nBits & 0xff );
    nBits >>= 8;
    nBitsCount -= 8;
    pBitPtr = pCurrent++;
  }
  CheckCurrentW();
}

inline void CBitStream::WriteBit(unsigned int _nBits)
{
  if (nBitsCount == 0)
  {
    pBitPtr = pCurrent++;
    nBits = _nBits;
    nBitsCount = 1;
  }
  else
  {
    nBits += (_nBits << nBitsCount);
    nBitsCount++;
  }
  if (nBitsCount > 8)
  {
    pBitPtr[0] = static_cast<unsigned char>(nBits);// ( nBits & 0xff );
    nBits >>= 8;
    nBitsCount -= 8;
    pBitPtr = pCurrent++;
  }
  CheckCurrentW();
}

inline unsigned int CBitStream::ReadBits(unsigned int _nBitsCount)
{
  while (nBitsCount < _nBitsCount)
  {
    nBits += static_cast<unsigned int>(*pCurrent++) << nBitsCount;
    nBitsCount += 8;
  }
  int nRes = nBits & nBitsMask[_nBitsCount - 1];
  nBits >>= _nBitsCount;
  nBitsCount -= _nBitsCount;
  CheckCurrentR();
  return nRes;
}

inline unsigned int CBitStream::ReadBit()
{
  if (nBitsCount < 1)
  {
    nBits = static_cast<unsigned int>(*pCurrent++);
    nBitsCount = 8;
  }
  int nRes = nBits & 1;
  nBits >>= 1;
  nBitsCount--;
  CheckCurrentR();
  return nRes;
}

#endif // __STREAMS_H__