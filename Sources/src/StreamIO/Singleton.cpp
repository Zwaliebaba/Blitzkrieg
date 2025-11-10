#include "StdAfx.h"

#ifndef __REDUCED_SINGLETON__
#include "OptionSystemInternal.h"
#include "GlobalVars.h"
#include "ConsoleBuffer.h"
#include "RandomGenInternal.h"
#endif // __REDUCED_SINGLETON__

// temp buffer
static std::vector<BYTE> tempbuffers[10];

struct STempBufferAutomatic
{
  STempBufferAutomatic()
  {
    for (int i = 0; i < 10; ++i) tempbuffers[i].resize(32);
  }
};

static STempBufferAutomatic tempinitautomagic;

void * STDCALL GetTempRawBuffer_Hook(int nSize, int nIndex)
{
  NI_ASSERT_SLOW_TF(nIndex < 10, "Can use only 10 temp buffers", return 0);
  tempbuffers[nIndex].reserve(nSize);
  return &(tempbuffers[nIndex][0]);
}
#ifdef __REDUCED_SINGLETON__
typedef void * (STDCALL *GETTEMPRAWBUFFER_HOOK)(int nAmount, int nBufferIndex);
GETTEMPRAWBUFFER_HOOK g_pfnGlobalGetTempRawBuffer = GetTempRawBuffer_Hook;
#endif // __REDUCED_SINGLETON__

class CSingleton : public ISingleton
{
  using CObjectIDs = std::hash_map<int, CPtr<IRefCount>>;
  CObjectIDs objects;

public:
  CSingleton();
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
#ifndef __REDUCED_SINGLETON__
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
#endif // __REDUCED_SINGLETON__
}

CSingleton theSingleton;
ISingleton * STDCALL GetSingletonGlobal_Hook() { return &theSingleton; }

#ifdef __REDUCED_SINGLETON__
ISingleton *g_pGlobalSingleton = &theSingleton;
#endif // __REDUCED_SINGLETON__

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
    if (it->second == pObj)
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