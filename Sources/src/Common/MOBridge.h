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
  bool STDCALL Create(IRefCount *pAIObj, const SGDBObjectDesc *pDesc, int nSeason, int nFrameIndex, float fHP, interface IVisObjBuilder *pVOB, IObjectsDB *pGDB) override;
  // placement
  void STDCALL SetPlacement(const CVec3 &vPos, const WORD &wDir) override;
  void STDCALL GetPlacement(CVec3 *pvPos, WORD *pwDir) override;
  // stats functions
  const SGDBObjectDesc * STDCALL GetDesc() const override;
  const SHPObjectRPGStats * STDCALL GetRPG() const override;
  // AI object retrieving
  IRefCount * STDCALL GetAIObj() override;
  IRefCount * STDCALL GetParentAIObj() override;
  //
  bool STDCALL CanSelect() const override;
  // get status for mission status bar
  void STDCALL GetStatus(struct SMissionStatusObject *pStatus) const override;
  // get actions, which this object can perform or actions, thi object can be acted with
  void STDCALL GetActions(CUserActions *pActions, EActionsType eActions) const override;
  // common updates
  void STDCALL AIUpdatePlacement(const struct SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) override;
  bool STDCALL AIUpdateRPGStats(const struct SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, IScene *pScene) override;
  void STDCALL AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) override;
  int STDCALL AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override { return 0; }
  // visiting
  void STDCALL Visit(IMapObjVisitor *pVisitor) override;
};

// //////////////////////////////////////////////////////////// 
#endif // __MOBRIDGE_H__