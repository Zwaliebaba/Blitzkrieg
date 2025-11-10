#ifndef __CLockWithUnlockPossibilities_h__
#define __CLockWithUnlockPossibilities_h__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "RectTiles.h"

// //////////////////////////////////////////////////////////// 
// in a given tile set remembers the locked state
// and saves it.
// after this it is possible to restore the initial locked state
class CAIUnit;

class CLockWithUnlockPossibilities
{
  DECLARE_SERIALIZE;

  void Unlock();
  void Lock();

  std::vector<BYTE> formerTilesType;
  CTilesSet pathTiles;

  bool bLocked;
  BYTE bAIClass;//
protected:
  SRect bigRect;// this entire Rect will be covered by the tank.
  bool TryLockAlongTheWay(bool bLock, BYTE _bAIClass);

public:
  CLockWithUnlockPossibilities()
    : bLocked(false) {}

  bool LockRect(const SRect &rect, const BYTE _bAIClass)
  {
    if (bLocked) { TryLockAlongTheWay(false, bAIClass); }
    bigRect = rect;
    bLocked = TryLockAlongTheWay(true, _bAIClass);
    return bLocked;
  }

  void UnlockIfLocked()
  {
    if (bLocked)
    {
      bLocked = false;
      TryLockAlongTheWay(false, bAIClass);
    }
  }
};

// //////////////////////////////////////////////////////////// 
#endif // __CLockWithUnlockPossibilities_h__