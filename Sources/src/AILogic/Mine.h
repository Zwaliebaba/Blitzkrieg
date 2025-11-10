#ifndef __MINE_H__
#define __MINE_H__

#pragma once

#include "StaticObject.h"

// **********************************************************************
// *CMineStaticObject*
// **********************************************************************

class CMineStaticObject : public CGivenPassabilityStObject
{
  OBJECT_COMPLETE_METHODS(CMineStaticObject);
  DECLARE_SERIALIZE;

  CGDBPtr<SMineRPGStats> pStats;
  int player;

  DWORD mVisibleStatus;
  NTimer::STime nextSegmTime;

  bool bIfWillBeDeleted;// one of the soldiers is heading towards this mine
  bool bIfRegisteredInCWorld;// mine visible

  bool bAlive;

  //
  void Detonate();

  // depends on the client
  bool IsRegisteredInWorld() const;

public:
  CMineStaticObject();
  CMineStaticObject(const SMineRPGStats *_pStats, const CVec2 &center, int dbID, float fHP, int nFrameIndex, int player);
  void Init() override;

  const SHPObjectRPGStats *GetStats() const override { return pStats; }

  void Segment() override;
  const NTimer::STime GetNextSegmentTime() const override { return nextSegmTime; }

  // if explodes under the given unit
  bool WillExplodeUnder(CAIUnit *pUnit);

  // detonate if the mine explodes when this unit collides; 
  bool CheckToDetonate(class CAIUnit *pUnit);
  void TakeDamage(float fDamage, bool bFromExplosion, int nPlayerOfShoot, CAIUnit *pShotUnit) override;
  void Die(float fDamage) override;
  EStaticObjType GetObjectType() const override { return ESOT_MINE; }

  const bool IsVisible(BYTE nParty) const override;
  void SetVisible(int nParty, bool bVis = true);

  // for removal by engineers
  bool IsBeingDisarmed() const { return bIfWillBeDeleted; }
  void SetBeingDisarmed(bool bStartDisarm);

  // depends on the client
  void RegisterInWorld();

  void ClearVisibleStatus();

  bool IsContainer() const override { return false; }
  const int GetNDefenders() const override { return 0; }
  class CSoldier *GetUnit(const int n) const override { return nullptr; }

  bool CanUnitGoThrough(const EAIClass &eClass) const override { return true; }

  bool IsAlive() const override { return bAlive; }
};

#endif // __MINE_H__