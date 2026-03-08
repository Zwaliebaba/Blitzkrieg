#ifndef __STREAM_ADAPTOR_H__
#define __STREAM_ADAPTOR_H__

#pragma once

class CStreamRangeAdaptor : public IDataStream
{
  OBJECT_NORMAL_METHODS(CStreamRangeAdaptor);
  //
  CPtr<IDataStream> pStream;// stream to adapt range
  std::string szName;// stream name
  SStorageElementStats stats;// own stats for sub-stream
  bool bHasOwnStats;// is the 'stats' valid
  // range begin/end
  int nBeginPos;
  int nEndPos;
  //
  int nSeekPos;// current seek position
public:
  CStreamRangeAdaptor() : bHasOwnStats(false), nBeginPos(0), nEndPos(0), nSeekPos(0) {}

  CStreamRangeAdaptor(IDataStream *_pStream, int _nBeginPos, int _nEndPos, const char *pszName = "", const SStorageElementStats *pStats = nullptr)
    : pStream(_pStream), szName(pszName), nBeginPos(_nBeginPos), nEndPos(_nEndPos), nSeekPos(0)
  {
    if (pStats)
    {
      stats = *pStats;
      stats.pszName = szName.c_str();
    }
    bHasOwnStats = pStats != nullptr;
  }

  // read/write data
  int STDCALL Read(void *pBuffer, int nLength) override
  {
    const int nLastPos = pStream->GetPos();
    pStream->Seek(nBeginPos + nSeekPos, STREAM_SEEK_SET);
    int nLengthToRead = Min(nEndPos - (nBeginPos + nSeekPos), nLength);
    nLengthToRead = pStream->Read(pBuffer, nLengthToRead);
    nSeekPos += nLengthToRead;
    pStream->Seek(nLastPos, STREAM_SEEK_SET);
    return nLengthToRead;
  }

  int STDCALL Write(const void *pBuffer, int nLength) override
  {
    const int nLastPos = pStream->GetPos();
    pStream->Seek(nBeginPos + nSeekPos, STREAM_SEEK_SET);
    const int nWrittenLength = pStream->Write(pBuffer, nLength);
    nSeekPos += nWrittenLength;
    pStream->Seek(nLastPos, STREAM_SEEK_SET);
    return nWrittenLength;
  }

  // declare the current position in the stream as the beginning of the stream
  int STDCALL LockBegin() override { return -1; }
  // return the start of the stream to the zero position
  int STDCALL UnlockBegin() override { return -1; }
  // current position in the stream
  int STDCALL GetPos() const override { return pStream->GetPos() - nBeginPos; }
  // set the current position in the stream
  int STDCALL Seek(int offset, STREAM_SEEK from) override
  {
    switch (from)
    {
      case STREAM_SEEK_SET:
        nSeekPos = offset;
        break;
      case STREAM_SEEK_CUR:
        nSeekPos += offset;
        break;
      case STREAM_SEEK_END:
        nSeekPos = nEndPos - nBeginPos + offset;
        break;
    }
    return nSeekPos;
  }

  // get stream size
  int STDCALL GetSize() const override { return nEndPos - nBeginPos; }
  // change stream size
  bool STDCALL SetSize(int nSize) override { return false; }
  // copy 'nLength' byte from current stream position to current 'pDstStream' stream position
  int STDCALL CopyTo(IDataStream *pDstStream, int nLength) override
  {
    if (nLength == 0) return 0;
    //
    std::vector<BYTE> buffer(nLength);
    nLength = Read(&(buffer[0]), nLength);
    return pDstStream->Write(&(buffer[0]), nLength);
  }

  // reset all cached data
  void STDCALL Flush() override {}
  // get information about the stream
  void STDCALL GetStats(SStorageElementStats *pStats) override
  {
    if (bHasOwnStats) *pStats = stats;
    else
    {
      pStream->GetStats(pStats);
      pStats->nSize = GetSize();
      pStats->pszName = szName.c_str();
    }
  }
};

class CStreamCOMAdaptor : public IStream
{
  int nRefCount;
  CPtr<IDataStream> pStream;

public:
  CStreamCOMAdaptor(IDataStream *_pStream) : nRefCount(1), pStream(_pStream) {}
  // IUnknown
  HRESULT STDCALL QueryInterface(REFIID iid, void **ppvObject) override
  {
    if (iid == IID_IUnknown) *ppvObject = static_cast<IUnknown *>(this);
    else if (iid == IID_IStream) *ppvObject = static_cast<IStream *>(this);
    else
    {
      *ppvObject = nullptr;
      return E_NOINTERFACE;
    }
    AddRef();
    return S_OK;
  }

  ULONG STDCALL AddRef() override { return ++nRefCount; }

  ULONG STDCALL Release() override
  {
    int nRef = --nRefCount;
    if (nRefCount == 0) delete this;
    return nRef;
  }

  // ISequentialStream
  HRESULT STDCALL Read(void *pv, ULONG cb, ULONG *pcbRead) override
  {
    int nLength = pStream->Read(pv, cb);
    if (pcbRead) *pcbRead = nLength;
    return S_OK;
  }

  HRESULT STDCALL Write(const void *pv, ULONG cb, ULONG *pcbWritten) override
  {
    int nLength = pStream->Write(pv, cb);
    if (pcbWritten) *pcbWritten = nLength;
    return S_OK;
  }

  // IStream itself
  HRESULT STDCALL Seek(LARGE_INTEGER dlibMove, DWORD dwOrigin, ULARGE_INTEGER *plibNewPosition) override
  {
    int nPos = pStream->Seek(static_cast<int>(dlibMove.QuadPart), static_cast<STREAM_SEEK>(dwOrigin));
    if (plibNewPosition) plibNewPosition->QuadPart = nPos;
    return S_OK;
  }

  HRESULT STDCALL SetSize(ULARGE_INTEGER libNewSize) override
  {
    pStream->SetSize(static_cast<int>(libNewSize.QuadPart));
    return S_OK;
  }

  HRESULT STDCALL CopyTo(IStream *pDst, ULARGE_INTEGER cb, ULARGE_INTEGER *pcbRead, ULARGE_INTEGER *pcbWritten) override
  {
    std::vector<BYTE> buffer(static_cast<int>(cb.QuadPart));
    int nRead = pStream->Read(&(buffer[0]), buffer.size());
    if (pcbRead) pcbRead->QuadPart = nRead;
    ULONG uWrite = 0;
    pDst->Write(&(buffer[0]), Min(nRead, static_cast<int>(buffer.size())), &uWrite);
    if (pcbWritten) pcbWritten->QuadPart = uWrite;
    return S_OK;
  }

  HRESULT STDCALL Commit(DWORD grfCommitFlags) override
  {
    pStream->Flush();
    return S_OK;
  }

  HRESULT STDCALL Revert() override { return S_OK; }

  HRESULT STDCALL LockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) override { return STG_E_INVALIDFUNCTION; }

  HRESULT STDCALL UnlockRegion(ULARGE_INTEGER libOffset, ULARGE_INTEGER cb, DWORD dwLockType) override { return STG_E_INVALIDFUNCTION; }

  HRESULT STDCALL Stat(STATSTG *pStats, DWORD grfStatFlag) override { return STG_E_ACCESSDENIED; }

  HRESULT STDCALL Clone(IStream **ppstm) override { return S_OK; }
};

#endif // __STREAM_ADAPTOR_H__