#ifndef __FLAG_H__
#define __FLAG_H__

#pragma once

#include "StaticObject.h"

class CFlag : public CCommonStaticObject
{
  OBJECT_COMPLETE_METHODS(CFlag);
  DECLARE_SERIALIZE;

  CGDBPtr<SStaticObjectRPGStats> pStats;

  int nParty;
  NTimer::STime nextSegmentTime;

  bool bGoingToCapture;
  int nPartyToCapture;
  int nPlayerToCapture;
  NTimer::STime timeOfStartCapturing;
  NTimer::STime lastSegmentTime;

  NTimer::STime timeOfStartNeutralPartyCapturing;
  bool bCapturingByNeutral;

public:
  CFlag() {}
  CFlag(const SStaticObjectRPGStats *pStats, const CVec2 &center, int dbID, float fHP, int nFrameIndex, int nPlayer, EStaticObjType eType);

  void Init() override;

  void TakeDamage(const float fDamage, const bool bFromExplosion, const int nPlayerOfShoot, CAIUnit *pShotUnit) override {}
  bool ProcessAreaDamage(const class CExplosion *pExpl, const int nArmorDir, const float fRadius, const float fSmallRadius) override { return true; }

  void Die(const float fDamage) override {}

  void Segment() override;

  const NTimer::STime GetNextSegmentTime() const override { return nextSegmentTime; }

  const BYTE GetPlayer() const override;
  const SHPObjectRPGStats *GetStats() const override { return pStats; }
  bool CanUnitGoThrough(const EAIClass &eClass) const override { return true; }
  bool ShouldSuspendAction(const EActionNotify &eAction) const override { return false; }
};

#endif // __FLAG_H__