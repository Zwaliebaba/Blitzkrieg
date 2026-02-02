#ifndef __MOBRIDGE_H__
#define __MOBRIDGE_H__
// //////////////////////////////////////////////////////////// 
#pragma once
// //////////////////////////////////////////////////////////// 
#include "MapObject.h"

// //////////////////////////////////////////////////////////// 
class CMOBridgeSpan : public CTRefCount<SBridgeSpanObject>
{
  OBJECT_SERVICE_METHODS(CMOBridgeSpan);
  DECLARE_SERIALIZE;
  //
  int nSpanType;// 0 - begin, 1 - center, 2 - end
  //
  const SBridgeRPGStats::SSpan &GetSpanStats(int nDamageState = 0) const;
  int GetSpanStatsIndex(int nDamageState = 0) const;
  void UpdateModelWithHP(float fNewHP, const NTimer::STime &currTime, IVisObjBuilder *pVOB) const;

public:
  bool Create(IRefCount *pAIObj, const SGDBObjectDesc *pDesc, int nSeason, int nFrameIndex, float fHP, interface IVisObjBuilder *pVOB, IObjectsDB *pGDB) override;
  // placement
  void SetPlacement(const CVec3 &vPos, const WORD &wDir) override;
  void GetPlacement(CVec3 *pvPos, WORD *pwDir) override;
  // stats functions
  const SGDBObjectDesc * GetDesc() const override;
  const SHPObjectRPGStats * GetRPG() const override;
  // AI object retrieving
  IRefCount * GetAIObj() override;
  IRefCount * GetParentAIObj() override;
  //
  bool CanSelect() const override;
  // get status for mission status bar
  void GetStatus(struct SMissionStatusObject *pStatus) const override;
  // get actions, which this object can perform or actions, thi object can be acted with
  void GetActions(CUserActions *pActions, EActionsType eActions) const override;
  // common updates
  void AIUpdatePlacement(const struct SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) override;
  bool AIUpdateRPGStats(const struct SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, IScene *pScene) override;
  void AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) override;
  int AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override { return 0; }
  // visiting
  void Visit(IMapObjVisitor *pVisitor) override;
};

// //////////////////////////////////////////////////////////// 
#endif // __MOBRIDGE_H__