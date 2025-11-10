#ifndef __MOOBJECT_H__
#define __MOOBJECT_H__
// //////////////////////////////////////////////////////////// 
#pragma once
// //////////////////////////////////////////////////////////// 
#include "MapObject.h"
#include "../Main/TextSystem.h"

// //////////////////////////////////////////////////////////// 
class CMOObject : public CTRefCount<SMapObject>
{
  OBJECT_SERVICE_METHODS(CMOObject);
  DECLARE_SERIALIZE;
  //
  DWORD actions[2];// actions, this object can do
  mutable CPtr<IText> pLocalName;// localized name of this object (ZB "√it's on the road")
  bool bDead;// is object already dead?
  std::string szFlagSide;
  static int nLastMarkerID;
  int nMarkerID;
  //
  void FillActions();
  bool AddAction(int nAction);
  //
  void UpdateModelWithHP(float fNewHP, IVisObjBuilder *pVOB);
  //
  IText *GetLocalName() const
  {
    if (pLocalName == nullptr) pLocalName = ::GetLocalName(pDesc);
    return pLocalName;
  }

public:
  bool STDCALL Create(IRefCount *pAIObj, const SGDBObjectDesc *pDesc, int nSeason, int nFrameIndex, float fHP, interface IVisObjBuilder *pVOB, IObjectsDB *pGDB) override;
  // placement
  void STDCALL SetPlacement(const CVec3 &vPos, const WORD &wDir) override;
  void STDCALL GetPlacement(CVec3 *pvPos, WORD *pwDir) override;
  // stats functions
  const SGDBObjectDesc * STDCALL GetDesc() const override { return pDesc; }
  const SHPObjectRPGStats * STDCALL GetRPG() const override { return pRPG; }
  // AI object retrieving
  IRefCount * STDCALL GetAIObj() override { return pAIObj; }
  IRefCount * STDCALL GetParentAIObj() override { return nullptr; }
  // get status for mission status bar
  void STDCALL GetStatus(struct SMissionStatusObject *pStatus) const override;
  // get actions, which this object can perform or actions, thi object can be acted with
  void STDCALL GetActions(CUserActions *pActions, EActionsType eActions) const override;
  // common updates
  void STDCALL AIUpdatePlacement(const SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) override;
  bool STDCALL AIUpdateRPGStats(const SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, IScene *pScene) override;
  int STDCALL AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override;
  void STDCALL AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) override;
  // visiting
  void STDCALL Visit(IMapObjVisitor *pVisitor) override;
};

// //////////////////////////////////////////////////////////// 
#endif // __MOOBJECT_H__