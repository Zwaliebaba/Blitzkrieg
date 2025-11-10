#ifndef __BASICCHUNK1_H_
#define __BASICCHUNK1_H_

#pragma once

#include "Streams.h"
#include "StructureSaver.h"
#include "../Misc/CheckSums.h"
// a) chunk structure
// b) ptr/ref storage
// system is able to store ref/ptr only for objectbase ancestors
// final save file structure
// -header section list of object types with pointers
// -object data separated in chunks one chunk per object
// c) can replace CMemoryStream with specialized objects to increase performance

// chunk with index 0 is used for system and should not be used in user code

#ifndef _FINALRELEASE
struct SObjectInfo
{
  IRefCount *pObj;// object itself
  int nUID;// object's unique ID
  uLong uCheckSum;// checksum (CRC32)
  int nSize;// object's size
  std::list<IRefCount *> referedObjects;// referred objects
  std::list<int> referedUIDs;
  //
  SObjectInfo() : nUID(0) {}
  explicit SObjectInfo(int _nUID) : nUID(_nUID) {}
};

inline bool operator<(const SObjectInfo &i1, const SObjectInfo &i2) { return i1.nUID < i2.nUID; }

class CFindObjGreaterUID
{
  const int nUID;

public:
  CFindObjGreaterUID(const int _nUID) : nUID(_nUID) {}
  bool operator()(const SObjectInfo &info) const { return info.nUID > nUID; }
};

class CFindObjByObj
{
  const IRefCount *pObj;

public:
  CFindObjByObj(const IRefCount *_pObj) : pObj(_pObj) {}
  bool operator()(const SObjectInfo &info) const { return info.pObj == pObj; }
};
#endif // _FINALRELEASE

class CStructureSaver2 : public IStructureSaver
{
  OBJECT_MINIMAL_METHODS(CStructureSaver2);

  //
  struct CChunkLevel
  {
    SSChunkID idChunk, idLastChunk;
    int nStart, nLength;
    int nChunkNumber;// chunk number in order for reading - used when writing/reading vector/list
    int nLastPos, nLastNumber;

    void ClearCache();
    void Clear();
    CChunkLevel() { Clear(); }
  };

  //
  CPtr<IDataStream> pDstStream;
  IObjectFactory *pFactory;
  IGDB *pGDB;
  // objects descriptors
  CMemoryStream obj;
  // objects data
  CMemoryStream data;
  std::list<CChunkLevel> chunks;
  using CChunkLevelIterator = std::list<CChunkLevel>::iterator;
  using CChunkLevelReverseIterator = std::list<CChunkLevel>::reverse_iterator;
  bool bIsReading;
  // maps objects addresses during save(first) to addresses during load(second) - during loading
  // or serves as a sign that some object has already been stored - during storing
  using CObjectsHash = std::unordered_map<void *, CPtr<IRefCount>, SDefaultPtrHash>;
  CObjectsHash objects;
  using CPObjectsHashSet = std::unordered_set<IRefCount *, SDefaultPtrHash>;
  CPObjectsHashSet storedObjects;
  std::list<CPtr<IRefCount>> toStore;
  //
#ifndef _FINALRELEASE
  bool bCheckResourcesOnLoad;
  NCheckSums::SCheckSumBufferStorage crcBuffer;
  bool bCalculateCRC;
  bool bCollectReferedObjects;
  using CObjectInfoList = std::vector<SObjectInfo>;
  CObjectInfoList objinfos;
  std::unordered_set<IRefCount *, SDefaultPtrHash> objset;
  std::list<IRefCount *> referedObjects;
#endif // _FINALRELEASE
  //
  bool ReadShortChunk(CChunkLevel &src, int &nPos, CChunkLevel &res);
  bool WriteShortChunk(CChunkLevel &dst, SSChunkID dwID, const unsigned char *pData, int nLength);
  bool GetShortChunk(CChunkLevel &src, SSChunkID dwID, CChunkLevel &res, int nNumber);
  int CountShortChunks(CChunkLevel &src, SSChunkID dwID);
  //
  void AlignDataFileSize();
  //
  void RawData(void *pData, int nSize);
  void WriteRawData(const void *pData, int nSize);
  //
  void Start(EAccessMode eAccessMode, interface IProgressHook *pHook);
  void Finish();

public:
  CStructureSaver2(IDataStream *pStream, EAccessMode eAccessMode,
                   interface IProgressHook *pLoadHook, IObjectFactory *_pFactory, IGDB *_pGDB);
  ~CStructureSaver2();
  //
  // start new complex chunk
  bool STDCALL StartChunk(SSChunkID idChunk) override;
  // finish complex chunk
  void STDCALL FinishChunk() override;
  // simply raw data chunk
  void STDCALL DataChunk(SSChunkID idChunk, void *pData, int nSize) override;
  // data stream as data chunk
  void STDCALL DataChunk(IDataStream *pStream) override;
  // count number of subchunks in the given chunk
  int STDCALL CountChunks(SSChunkID idChunk) override;
  // set subchunks number in the given chunk
  void STDCALL SetChunkCounter(int nCount) override { chunks.back().nChunkNumber = nCount; }
  // is structure saver opened in the READ mode?
  bool STDCALL IsReading() const override { return bIsReading; }
  // loading an object and recreating it
  IRefCount * STDCALL LoadObject() override;
  // recording the object and data needed to recreate it when loaded
  void STDCALL StoreObject(IRefCount *pObj) override;
  // get a pointer to the game database
  interface IGDB * STDCALL GetGDB() override { return pGDB; }
};

#endif