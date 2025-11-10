#ifndef __FENCE_H__
#define __FENCE_H__

#pragma once

#include "StaticObject.h"

class CFence : public CCommonStaticObject
{
  OBJECT_COMPLETE_METHODS(CFence);
  DECLARE_SERIALIZE;

  CGDBPtr<SFenceRPGStats> pStats;

  int nCreator;// diplomacy of creator

  int nDir;
  SVector leftTile, rightTile;
  bool bSuspendAppear;

  enum ETypesOfLife { ETOL_SAFE, ETOL_LEFT, ETOL_RIGHT, ETOL_DESTROYED };

  ETypesOfLife eLifeType;

  // neighboring fences
  std::list<CPtr<CFence>> neighFences;
  // parties whose fences break down
  std::list<ETypesOfLife> dirToBreak;

  //
  void InitDirectionInfo();
  void AnalyzeConnection(CFence *pFence);
  void DamagePartially(ETypesOfLife eType);

public:
  CFence() {}
  CFence(const SFenceRPGStats *pStats, const CVec2 &center, int dbID, float fHP, int nFrameIndex, int nDiplomacy = -1, bool IsEditor = false);
  void Init() override;

  const BYTE GetPlayer() const override { return nCreator; }

  const struct SHPObjectRPGStats *GetStats() const override { return pStats; }

  EStaticObjType GetObjectType() const override { return ESOT_FENCE; }

  void Die(float fDamage) override;
  void Delete() override;

  bool CanUnitGoThrough(const EAIClass &eClass) const override;
  bool IsAlive() const override { return eLifeType != ETOL_DESTROYED && CCommonStaticObject::IsAlive(); }
  bool ShouldSuspendAction(const EActionNotify &eAction) const override;
};

#endif // __FENCE_H__