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
  bool Create(IRefCount *pAIObj, const SGDBObjectDesc *pDesc, int nSeason, int nFrameIndex, float fHP, interface IVisObjBuilder *pVOB, IObjectsDB *pGDB) override;
  bool Create(IRefCount *pAIObj, const char *pszName, interface IVisObjBuilder *pVOB) override;
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
  void GetStatus(struct SMissionStatusObject *pStatus) const override {}
  // get actions, which this object can perform or actions, thi object can be acted with
  void GetActions(CUserActions *pActions, EActionsType eActions) const override {}
  // common updates
  void AIUpdatePlacement(const SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) override;
  bool AIUpdateRPGStats(const SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, IScene *pScene) override { return true; }
  int AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override;
  void AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) override {}
  // visiting
  void Visit(IMapObjVisitor *pVisitor) override;
};

// //////////////////////////////////////////////////////////// 
#endif // __MOPROJECTILE_H__