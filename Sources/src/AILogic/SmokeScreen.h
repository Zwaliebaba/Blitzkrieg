#ifndef __SMOKE_SCREEN_H__
#define __SMOKE_SCREEN_H__
// ///////////////////////////////////////////////////////////// 
#pragma once
// ///////////////////////////////////////////////////////////// 
#include "StaticObject.h"

// ///////////////////////////////////////////////////////////// 
class CSmokeScreen : public CExistingObject
{
  OBJECT_COMPLETE_METHODS(CSmokeScreen);
  DECLARE_SERIALIZE;

  CVec2 vCenter;
  SVector tileCenter;
  float fRadius;
  int nTransparency;
  NTimer::STime timeOfDissapear;

  NTimer::STime nextSegmTime;
  bool bTransparencySet;

  //
  void OctupleTrace(int x, int y, bool bAdd);
  void TraceToPoint(int x, int y, bool bAdd);
  void Trace(bool bAdd);

protected:
  void SetNewPlaceWithoutMapUpdate(const CVec2 &center, const WORD dir = 0) override {}

public:
  CSmokeScreen() : bTransparencySet(false) {}
  CSmokeScreen(const CVec2 &vCenter, float fRadius, int nTransparency, int nTime);
  virtual void Init();

  const SHPObjectRPGStats *GetStats() const override { return nullptr; }

  void Segment() override;
  const NTimer::STime GetNextSegmentTime() const override { return nextSegmTime; }

  // detonate if the mine explodes when this unit collides; 
  void TakeDamage(const float fDamage, const bool bFromExplosion, const int nPlayerOfShoot, CAIUnit *pShotUnit) override {}
  void Die(const float fDamage) override {}
  EStaticObjType GetObjectType() const override { return ESOT_SMOKE_SCREEN; }

  bool IsContainer() const override { return false; }
  const int GetNDefenders() const override { return 0; }
  class CSoldier *GetUnit(const int n) const override { return nullptr; }

  bool CanUnitGoThrough(const EAIClass &eClass) const override { return true; }

  void LockTiles(bool bInitialization = false) override {}
  void UnlockTiles(bool bInitialization = false) override {}
  void SetTransparencies() override;
  void RemoveTransparencies() override;
  void RestoreTransparencies() override;

  void GetCoveredTiles(CTilesSet *pTiles) const override;

  void GetRPGStats(struct SAINotifyRPGStats *pStats) override {}
  const CVec2 &GetCenter() const override { return vCenter; }
  const CVec2 GetAttackCenter(const CVec2 &vPoint) const override { return vCenter; }
  void GetBoundRect(SRect *pRect) const override { pRect->InitRect(GetCenter(), CVec2(1.0f, 1.0f), 0.0f, 0.0f); }
  bool IsPointInside(const CVec2 &point) const override { return false; }
  const WORD GetDir() const override { return 0; }
  bool CanBeMovedTo(const CVec2 &newCenter) const override { return false; }
};

// ///////////////////////////////////////////////////////////// 
#endif // __SMOKE_SCREEN_H__