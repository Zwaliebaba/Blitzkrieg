#ifndef __MOENTRENCHMENT_H__
#define __MOENTRENCHMENT_H__
// //////////////////////////////////////////////////////////// 
#pragma once
// //////////////////////////////////////////////////////////// 
#include "MapObject.h"
#include "../Main/TextSystem.h"
#include "../Anim/Animation.h"
#include "../Scene/Scene.h"

// //////////////////////////////////////////////////////////// 
class CMOEntrenchmentSegment : public CTRefCount<SMapObject>
{
  OBJECT_SERVICE_METHODS(CMOEntrenchmentSegment);
  DECLARE_SERIALIZE;
  //
  mutable CPtr<IText> pLocalName;// localized name of this object (ZB "√it's on the road")
  //
  const SEntrenchmentRPGStats *GetRPGStats() const { return static_cast_gdb<const SEntrenchmentRPGStats *>(pRPG); }
  IMeshVisObj *GetVisObj() { return static_cast_ptr<IMeshVisObj *>(pVisObj); }
  IMeshAnimation *GetAnim() { return static_cast<IMeshAnimation *>(GetVisObj()->GetAnimation()); }
  //
  IText *GetLocalName() const
  {
    if (pLocalName == nullptr) pLocalName = ::GetLocalName(pDesc);
    return pLocalName;
  }

  void UpdateModelWithHP(float fNewHP, IVisObjBuilder *pVOB);

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
  int STDCALL AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override { return 0; }
  void STDCALL AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) override;
  // visiting
  void STDCALL Visit(IMapObjVisitor *pVisitor) override;
};

// //////////////////////////////////////////////////////////// 
#endif // __MOENTRENCHMENT_H__