#ifndef __BULLETSTORAGE_H__
#define __BULLETSTORAGE_H__

#pragma once

#include "StaticObject.h"

class CAIUnit;

// a box of cartridges that lies near the cannon.
// the artillerymen run to him and back for ammunition.
class CArtilleryBulletStorage : public CGivenPassabilityStObject
{
  OBJECT_COMPLETE_METHODS(CArtilleryBulletStorage);
  DECLARE_SERIALIZE;

  CGDBPtr<SStaticObjectRPGStats> pStats;

  CAIUnit *pOwner;

public:
  CArtilleryBulletStorage() : pOwner(nullptr) {}
  CArtilleryBulletStorage(const SStaticObjectRPGStats *pStats, const CVec2 &center, int dbID, float fHP, int nFrameIndex, CAIUnit *pOwner);

  const SHPObjectRPGStats *GetStats() const override { return pStats; }

  void TakeDamage(const float fDamage, const bool bFromExplosion, const int nPlayerOfShoot, CAIUnit *pShotUnit) override {}
  void Die(const float fDamage) override {}
  void TakeEditorDamage(const float fDamage) override {}
  void Segment() override {}

  void MoveTo(const CVec2 &newCenter);

  EStaticObjType GetObjectType() const override { return ESOT_ARTILLERY_BULLET_STORAGE; }

  bool IsContainer() const override { return false; }
  const int GetNDefenders() const override { return 0; }
  class CSoldier *GetUnit(const int n) const override { return nullptr; }

  bool CanUnitGoThrough(const EAIClass &eClass) const override { return true; }

  CAIUnit *GetOwner() const { return pOwner; }
};

#endif // __BRIDGE_H__