#ifndef __MOUNITINFANTRY_H__
#define __MOUNITINFANTRY_H__

#pragma once

#include "MOUnit.h"

class CMOUnitInfantry : public CMOUnit
{
  OBJECT_SERVICE_METHODS(CMOUnitInfantry);
  DECLARE_SERIALIZE;
  //
  CPtr<IMOSquad> pSquad;// squad, this soldier in
  int nDeadCounter;// unit dead counter
  CVec3 vSunDir;
  float fTraceProbabilityCoeff;
  float fTraceSpeedCoeff;
  //
  ISpriteVisObj *GetVisObj() { return static_cast_ptr<ISpriteVisObj *>(pVisObj); }
  ISpriteAnimation *GetAnim() { return static_cast<ISpriteAnimation *>(GetVisObj()->GetAnimation()); }
  const SInfantryRPGStats *GetRPGStats() const { return static_cast_gdb<const SInfantryRPGStats *>(pRPG); }
  //
  const CVec3 GetIconAddValue() const override { return CVec3(0, 0, 5); }
  void UpdateHPBarVisibility(float fHP);
  void MakeVisible(bool bVisible) override;
  bool ChangeModeWithBlood(const std::string &szModelName);
  void UpdateVisibility();

public:
  CMOUnitInfantry();
  //
  bool STDCALL Create(IRefCount *pAIObj, const SGDBObjectDesc *pDesc, int nSeason, int nFrameIndex, float fHP, interface IVisObjBuilder *pVOB, IObjectsDB *pGDB) override;
  // get status for mission status bar
  void STDCALL GetStatus(struct SMissionStatusObject *pStatus) const override;
  //
  void STDCALL SetSquad(interface IMOSquad *_pSquad) override;
  IMOSquad * STDCALL GetSquad() override { return pSquad; }
  // change selection state for this object
  void STDCALL Select(ISelector *pSelector, bool bSelect, bool bSelectSuper) override;
  //
  void STDCALL SetContainer(IMOContainer *_pContainer) override;
  //
  bool STDCALL Load(interface IMOUnit *pMO, bool bEnter) override { return false; }
  void STDCALL UpdatePassangers() override {}
  // get all passangers from container. 
  int STDCALL GetPassangers(IMOUnit **pBuffer, const bool bCanSelectOnly) const override { return 0; }
  // get free places
  int STDCALL GetFreePlaces() const override { return 0; }
  // get actions, which this object can perform or actions, thi object can be acted with
  void STDCALL GetActions(CUserActions *pActions, EActionsType eActions) const override;
  // common updates
  void STDCALL AIUpdatePlacement(const struct SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) override;
  bool STDCALL AIUpdateRPGStats(const SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, IScene *pScene) override;
  void STDCALL AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) override;
  //
  int STDCALL AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override;
  IMapObj * STDCALL AIUpdateFireWithProjectile(const SAINotifyNewProjectile &projectile, const NTimer::STime &currTime, interface IVisObjBuilder *pVOB) override;
  void STDCALL AIUpdateShot(const struct SAINotifyBaseShot &shot, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene) override;
  // visiting
  void STDCALL Visit(IMapObjVisitor *pVisitor) override;
  // change look with blood settings
  bool STDCALL ChangeWithBlood(IVisObjBuilder *pVOB) override;
  virtual void STDCALL SetHPSimpleBar(bool bSimple = true);
};

#endif // __MOUNITINFANTRY_H__