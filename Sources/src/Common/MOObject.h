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
  bool Create(IRefCount *pAIObj, const SGDBObjectDesc *pDesc, int nSeason, int nFrameIndex, float fHP, interface IVisObjBuilder *pVOB, IObjectsDB *pGDB) override;
  // placement
  void SetPlacement(const CVec3 &vPos, const WORD &wDir) override;
  void GetPlacement(CVec3 *pvPos, WORD *pwDir) override;
  // stats functions
  const SGDBObjectDesc * GetDesc() const override { return pDesc; }
  const SHPObjectRPGStats * GetRPG() const override { return pRPG; }
  // AI object retrieving
  IRefCount * GetAIObj() override { return pAIObj; }
  IRefCount * GetParentAIObj() override { return nullptr; }
  // get status for mission status bar
  void GetStatus(struct SMissionStatusObject *pStatus) const override;
  // get actions, which this object can perform or actions, thi object can be acted with
  void GetActions(CUserActions *pActions, EActionsType eActions) const override;
  // common updates
  void AIUpdatePlacement(const SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) override;
  bool AIUpdateRPGStats(const SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, IScene *pScene) override;
  int AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override;
  void AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) override;
  // visiting
  void Visit(IMapObjVisitor *pVisitor) override;
};

// //////////////////////////////////////////////////////////// 
#endif // __MOOBJECT_H__