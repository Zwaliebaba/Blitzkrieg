#ifndef __MOPROJECTILE_H__
#define __MOPROJECTILE_H__
// //////////////////////////////////////////////////////////// 
#pragma once
// //////////////////////////////////////////////////////////// 
#include "MapObject.h"

// //////////////////////////////////////////////////////////// 
class CMOProjectile : public CTRefCount<IMOEffect>
{
  OBJECT_SERVICE_METHODS(CMOProjectile);
  DECLARE_SERIALIZE;
  //
  float fTimeStart;// start time of this projectile
  float fTimeDuration;// duration of this effect
  CVec3 delta;// difference between object's center and real gun fire point
  WORD wMoveSoundID;// projectile movement sound
  // data for smooth particles path generation
  CVec3 vLastPos;// last update position
  NTimer::STime timeLastTime;// last update time
  //
  IEffectVisObj *GetVisObj() { return static_cast_ptr<IEffectVisObj *>(pVisObj); }

public:
  CMOProjectile();
  ~CMOProjectile() override;
  //
  void Init(const NTimer::STime &_timeStart, const NTimer::STime &_timeDuration, const CVec3 &_delta);
  //
  bool STDCALL Create(IRefCount *pAIObj, const SGDBObjectDesc *pDesc, int nSeason, int nFrameIndex, float fHP, interface IVisObjBuilder *pVOB, IObjectsDB *pGDB) override;
  bool STDCALL Create(IRefCount *pAIObj, const char *pszName, interface IVisObjBuilder *pVOB) override;
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
  void STDCALL GetStatus(struct SMissionStatusObject *pStatus) const override {}
  // get actions, which this object can perform or actions, thi object can be acted with
  void STDCALL GetActions(CUserActions *pActions, EActionsType eActions) const override {}
  // common updates
  void STDCALL AIUpdatePlacement(const SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) override;
  bool STDCALL AIUpdateRPGStats(const SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, IScene *pScene) override { return true; }
  int STDCALL AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override;
  void STDCALL AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) override {}
  // visiting
  void STDCALL Visit(IMapObjVisitor *pVisitor) override;
};

// //////////////////////////////////////////////////////////// 
#endif // __MOPROJECTILE_H__