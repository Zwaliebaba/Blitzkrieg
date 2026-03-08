#include "StdAfx.h"

#include "OptionSystemInternal.h"
#include "GlobalVars.h"
#include "ConsoleBuffer.h"
#include "RandomGenInternal.h"

// temp buffer
static std::vector<BYTE> tempbuffers[10];

void * STDCALL GetTempRawBuffer_Hook(int nSize, int nIndex)
{
  static bool initialized = []() {
    for (int i = 0; i < 10; ++i) tempbuffers[i].resize(32);
    return true;
  }();
  NI_ASSERT_SLOW_TF(nIndex < 10, "Can use only 10 temp buffers", return 0);
  tempbuffers[nIndex].reserve(nSize);
  return &(tempbuffers[nIndex][0]);
}

class CSingleton : public ISingleton
{
  using CObjectIDs = std::unordered_map<int, CPtr<IRefCount>>;
  CObjectIDs objects;

public:
  CSingleton();
  ~CSingleton();
  // register singleton object for global access
  bool STDCALL Register(int nID, IRefCount *pObj) override;
  // unregister singleton object by ID
  bool STDCALL UnRegister(int nID) override;
  // unregister singleton object by pointer
  bool STDCALL UnRegister(IRefCount *pObj) override;
  // get singleton object by ID
  IRefCount * STDCALL Get(int nID) override;
  // get all registered objects
  int STDCALL GetAllObjects(IRefCount ***pBuffer, int *pnBufferSize) override;
  // done - release all objects
  void STDCALL Done() override { objects.clear(); }
};

CSingleton::CSingleton()
{
  // create and register options system
  Register(IOptionSystem::tidTypeID, new COptionSystem());
  // create and register global vars system
  Register(IGlobalVars::tidTypeID, new CGlobalVars());
  // console buffer
  Register(IConsoleBuffer::tidTypeID, new CConsoleBuffer());
  // random generator
  {
    auto pRandomGen = new CRandomGenerator();
    Register(IRandomGen::tidTypeID, pRandomGen);
  }
}

CSingleton::~CSingleton()
{
  Done();
}

// Construct On First Use (Meyers' Singleton) — guarantees theSingleton is fully
// constructed before any code accesses it, regardless of static init order.
ISingleton * STDCALL GetSingletonGlobal_Hook()
{
  static CSingleton theSingleton;
  return &theSingleton;
}

bool CSingleton::Register(int nID, IRefCount *pObj)
{
  CObjectIDs::const_iterator pos = objects.find(nID);
  NI_ASSERT_TF(pos == objects.end(), NStr::Format("object 0x%x already registered", nID), return false);
  objects[nID] = pObj;
  return true;
}

bool CSingleton::UnRegister(int nID)
{
  CObjectIDs::iterator pos = objects.find(nID);
  if (pos != objects.end()) objects.erase(pos);
  return true;
}

bool CSingleton::UnRegister(IRefCount *pObj)
{
  for (CObjectIDs::iterator it = objects.begin(); it != objects.end(); ++it)
  {
    if (it->second.GetPtr() == pObj)
    {
      objects.erase(it);
      return true;
    }
  }
  return false;
}

IRefCount *CSingleton::Get(int nID)
{
  CObjectIDs::iterator pos = objects.find(nID);
  // NI_ASSERT_SLOW_TF( pos != objects.end(), NStr::Format("object with id = 0x%x does not registered", nID), return false );
  return pos == objects.end() ? nullptr : pos->second;
}

// get all registered objects
int CSingleton::GetAllObjects(IRefCount ***ppBuffer, int *pnBufferSize)
{
  NI_ASSERT_TF((ppBuffer != nullptr) && (pnBufferSize != nullptr), "NULL pointer passed to request", return -1);
  *pnBufferSize = objects.size();
  *ppBuffer = GetTempBuffer<IRefCount *>(*pnBufferSize);
  IRefCount **pBuffer = *ppBuffer;
  for (CObjectIDs::iterator it = objects.begin(); it != objects.end(); ++it) *pBuffer++ = it->second;
  return *pnBufferSize;
}