#ifndef __MOBUILDING_H__
#define __MOBUILDING_H__

#pragma once

#include "MapObject.h"
#include "../Anim/Animation.h"
#include "../Main/TextSystem.h"
#include "Passangers.h"

class CMOBuilding : public CTRefCount<IMOContainer>
{
  OBJECT_SERVICE_METHODS(CMOBuilding);
  DECLARE_SERIALIZE;
  //
  CPassangersList passangers;
  mutable CPtr<IText> pLocalName;// localized name of this building (ZB "-Ussky toilet")
  int nSeason;// season
  CPtr<IVisObj> pGarbage;// garbage around damaged or destroyed building
  float fTraceSpeedCoeff;
  float fTraceProbabilityCoeff;
  //
  ISpriteVisObj *GetVisObj() { return static_cast_ptr<ISpriteVisObj *>(pVisObj); }
  ISpriteAnimation *GetAnim() { return static_cast<ISpriteAnimation *>(GetVisObj()->GetAnimation()); }
  const SBuildingRPGStats *GetRPGStats() const { return static_cast_gdb<const SBuildingRPGStats *>(pRPG); }
  //
  void SetIcon(int nType, IVisObjBuilder *pVOB);
  void RemoveIcon(int nType);
  //
  int GetNumTotalSlots() const { return GetRPGStats()->nMedicalSlots + GetRPGStats()->nRestSlots + GetRPGStats()->slots.size(); }
  int GetNumFreeSlots() const { return GetNumTotalSlots() - passangers.size(); }
  bool CanEnterOrBoard() const;
  //
  bool UpdateModelWithHP(float fNewHP, IVisObjBuilder *pVOB, bool bForced = false);
  void AddEffectsAtDamagePoints(int nDamageState, const NTimer::STime &currTime, const NTimer::STime &timePassed, IVisObjBuilder *pVOB, IScene *pScene);
  //
  IText *GetLocalName() const
  {
    if (pLocalName == nullptr) pLocalName = ::GetLocalName(pDesc);
    return pLocalName;
  }

  //
  bool IsDOT() const { return GetRPGStats()->eType == SBuildingRPGStats::TYPE_DOT; }
  //
  ~CMOBuilding() override;

  void UpdateGunTraces(const CVec3 &vStart, const CVec3 &vEnd, float fSpeed, NTimer::STime nCurrTime, IScene *pScene);

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
  // check, is this object selected?
  bool STDCALL IsSelected() const override { return pVisObj->GetSelectionState() == SGVOSS_SELECTED; }
  // change selection state for this object
  void STDCALL Select(ISelector *pSelector, bool bSelect, bool bSelectSuper) override;
  virtual void STDCALL Select(int nSelectionState) { pVisObj->Select(static_cast<EVisObjSelectionState>(nSelectionState)); }
  //
  bool STDCALL Load(interface IMOUnit *pMO, bool bEnter) override;
  // show icons of the passangers
  void STDCALL UpdatePassangers() override;
  // get all passangers from container. 
  int STDCALL GetPassangers(IMOUnit **pBuffer, bool bCanSelectOnly) const override;
  // get free places
  int STDCALL GetFreePlaces() const override { return GetNumFreeSlots(); }
  // get actions, which this object can perform or actions, thi object can be acted with
  void STDCALL GetActions(CUserActions *pActions, EActionsType eActions) const override;
  // common updates
  int STDCALL AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override;
  void STDCALL AIUpdatePlacement(const SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) override;
  bool STDCALL AIUpdateRPGStats(const SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, IScene *pScene) override;
  bool STDCALL AIUpdateDiplomacy(const SAINotifyDiplomacy &diplomacy) override;
  void STDCALL AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) override;
  // firing... (from container of by himself)
  void STDCALL AIUpdateShot(const struct SAINotifyBaseShot &shot, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene) override;
  // visiting
  void STDCALL Visit(IMapObjVisitor *pVisitor) override;
};

#endif // __MOBUILDING_H__