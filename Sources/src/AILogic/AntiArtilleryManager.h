#ifndef __ANTI_ARTILLERY_MANAGER_H__
#define __ANTI_ARTILLERY_MANAGER_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "Heap.h"
#include "AIHashFuncs.h"
// //////////////////////////////////////////////////////////// 
class CAntiArtillery;

// //////////////////////////////////////////////////////////// 
class CAntiArtilleryManager
{
  DECLARE_SERIALIZE;

  // circles from shots from own artillery for each side
  using CAntiArtilleries = std::hash_set<int>;
  std::vector<CAntiArtilleries> antiArtilleries;

  static bool IsHeardForParty(CAntiArtillery *pAntiArt, int nParty);

public:
  void Init();
  void Clear();

  void AddAA(CAntiArtillery *pAA);
  void RemoveAA(CAntiArtillery *pAA);
  void Segment();

  // won't save!
  class CIterator
  {
    int nIterParty;
    int nCurParty;
    CAntiArtilleries::iterator curIter;

  public:
    CIterator(int nParty);

    const CCircle operator*() const;
    CAntiArtillery *GetAntiArtillery() const;

    void Iterate();
    bool IsFinished() const;
  };

  friend class CIterator;
};

// //////////////////////////////////////////////////////////// 
#endif // __ANTI_ARTILLERY_MANAGER_H__