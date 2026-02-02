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
  // check, is this object selected?
  bool IsSelected() const override { return pVisObj->GetSelectionState() == SGVOSS_SELECTED; }
  // change selection state for this object
  void Select(ISelector *pSelector, bool bSelect, bool bSelectSuper) override;
  virtual void Select(int nSelectionState) { pVisObj->Select(static_cast<EVisObjSelectionState>(nSelectionState)); }
  //
  bool Load(interface IMOUnit *pMO, bool bEnter) override;
  // show icons of the passangers
  void UpdatePassangers() override;
  // get all passangers from container. 
  int GetPassangers(IMOUnit **pBuffer, bool bCanSelectOnly) const override;
  // get free places
  int GetFreePlaces() const override { return GetNumFreeSlots(); }
  // get actions, which this object can perform or actions, thi object can be acted with
  void GetActions(CUserActions *pActions, EActionsType eActions) const override;
  // common updates
  int AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override;
  void AIUpdatePlacement(const SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) override;
  bool AIUpdateRPGStats(const SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, IScene *pScene) override;
  bool AIUpdateDiplomacy(const SAINotifyDiplomacy &diplomacy) override;
  void AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) override;
  // firing... (from container of by himself)
  void AIUpdateShot(const struct SAINotifyBaseShot &shot, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene) override;
  // visiting
  void Visit(IMapObjVisitor *pVisitor) override;
};

#endif // __MOBUILDING_H__