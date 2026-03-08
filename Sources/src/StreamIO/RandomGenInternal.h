#ifndef __RANDOMGENINTERNAL_H__
#define __RANDOMGENINTERNAL_H__

#pragma once

#include "RandomGen.h"

constexpr int RANDSIZL = 8;
constexpr int RANDSIZ = 1 << RANDSIZL;

struct SRandData
{
  unsigned _int32 randcnt;
  unsigned _int32 randrsl[RANDSIZ];
  unsigned _int32 randmem[RANDSIZ];
  unsigned _int32 randa;
  unsigned _int32 randb;
  unsigned _int32 randc;
};

class CRandomGenSeed : public CTRefCount<IRandomGenSeed>
{
  OBJECT_SERVICE_METHODS(CRandomGenSeed);
  DECLARE_SERIALIZE;
  //
  SRandData rnd;
  //
  bool RecFindFile(LPSTR pszFindedName, LPCSTR pszBaseMask, int nToFind, int *pnTotFinded);
  void FillRandRsl();
  void InitVariables();

public:
  void STDCALL Init() override;
  void STDCALL InitByZeroSeed() override;

  //
  const SRandData &GetRandData() const { return rnd; }
  void SetRandData(const SRandData &_rnd) { rnd = _rnd; }
  //
  int STDCALL operator&(IDataTree &ss) override;
  //
  void STDCALL Store(IDataStream *pStream) override;
  void STDCALL Restore(IDataStream *pStream) override;
};

void Isaac(SRandData *pRnd);

class CRandomGenerator : public CTRefCount<IRandomGen>
{
  OBJECT_SERVICE_METHODS(CRandomGenerator);
  DECLARE_SERIALIZE;
  //
  BOOL bIsReady;
  SRandData rnd;
  //
public:
  CRandomGenerator() { bIsReady = FALSE; }
  // Initialization 
  void STDCALL Init() override;
  void STDCALL SetSeed(IRandomGenSeed *pSeed) override;
  IRandomGenSeed * STDCALL GetSeed() override;
  //
  unsigned int STDCALL Get() override
  {
    if (rnd.randcnt-- == 0)
    {
      Isaac(&rnd);
      rnd.randcnt = RANDSIZ - 1;
    }
    return rnd.randrsl[rnd.randcnt];
  }

  void STDCALL Store(IDataStream *pStream) override;
  void STDCALL Restore(IDataStream *pStream) override;

  unsigned _int32 Get(unsigned _int32 nMax) { return Get() % (nMax + 1); }
  unsigned _int32 Get(unsigned _int32 nMin, unsigned _int32 nMax) { return Get(nMax - nMin) + nMin; }
  float GetFloat(float fMin, float fMax) { return fMin + static_cast<float>(double(Get()) * double(fMax - fMin) / double(0xFFFFFFFF)); }

  unsigned _int32 Dice(unsigned _int32 nNum, unsigned _int32 nDice)
  {
    unsigned _int32 nRes = 0;
    for (unsigned _int32 i = 0; i < nNum; i++) nRes += Get() % nDice + 1;
    return nRes;
  }
};

#endif // __RANDOMGENINTERNAL_H__