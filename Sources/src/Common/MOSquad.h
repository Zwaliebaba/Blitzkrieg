#ifndef __MOSQUAD_H__
#define __MOSQUAD_H__
// //////////////////////////////////////////////////////////// 
#pragma once
// //////////////////////////////////////////////////////////// 
#include "MapObject.h"
#include "Actions.h"

// //////////////////////////////////////////////////////////// 
class CMOSquad : public CTRefCount<IMOSquad>
{
  OBJECT_SERVICE_METHODS(CMOSquad);
  DECLARE_SERIALIZE;

  //
  struct SUnitDesc
  {
    CObj<IMOUnit> pUnit;// unit itself
    float fHP;// HP [0..1]
    float fAmmo1;// primary ammo [0..1]
    float fAmmo2;// secondary ammo [0..1]
    //
    SUnitDesc() {}

    SUnitDesc(IMOUnit *_pUnit, const float _fHP)
      : pUnit(_pUnit), fHP(_fHP), fAmmo1(1), fAmmo2(1) {}

    int operator&(IStructureSaver &ss)
    {
      CSaverAccessor saver = &ss;
      saver.Add(1, &pUnit);
      saver.Add(2, &fHP);
      saver.Add(3, &fAmmo1);
      saver.Add(4, &fAmmo2);
      return 0;
    }
  };

  // visual data
  CPtr<ISquadVisObj> pSquadVisObj;
  //
  using CUnitsList = std::list<SUnitDesc>;
  CUnitsList passangers;// all infantry units in the squad
  SAINotifyDiplomacy suspendedDiplomacy;
  //
  const int GetSelectionState() const { return IsSelected() ? SGVOSS_SELECTED : SGVOSS_UNSELECTED; }

  SUnitDesc *GetUnit(IMOUnit *pUnit)
  {
    for (auto it = passangers.begin(); it != passangers.end(); ++it) { if (it->pUnit.GetPtr() == pUnit) return &(*it); }
    return nullptr;
  }

  //
  void UpdateVisObj();
  //
  CMOSquad();
  ~CMOSquad() override;

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
  //
  bool CanSelect() const override { return passangers.empty() ? false : passangers.back().pUnit->CanSelect(); }
  // get status for mission status bar
  void GetStatus(struct SMissionStatusObject *pStatus) const override {}
  // get actions, which this object can perform or actions, thi object can be acted with
  void GetActions(CUserActions *pActions, EActionsType eActions) const override;
  // common updates
  int AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override { return 0; }
  void AIUpdatePlacement(const struct SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) override {}
  bool AIUpdateRPGStats(const struct SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, IScene *pScene) override { return true; }
  bool AIUpdateDiplomacy(const struct SAINotifyDiplomacy &diplomacy) override;
  void AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) override {}
  // firing... (from container of by himself)
  void AIUpdateShot(const struct SAINotifyBaseShot &shot, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene) override {}
  // visiting
  void Visit(IMapObjVisitor *pVisitor) override {}
  // check, is this object selected?
  bool IsSelected() const override;
  // change selection state for this object
  void Select(ISelector *pSelector, bool bSelect, bool bSelectSuper) override;
  // load unit onboard or unload it
  bool Load(interface IMOUnit *pMO, bool bEnter) override;
  //
  void UpdatePassangers() override {}
  // get all passangers from container. 
  int GetPassangers(IMOUnit **pBuffer, bool bCanSelectOnly) const override;
  // get free places
  int GetFreePlaces() const override { return 0; }
  // notify about RPG stats changing fot the single squad member
  void NotifyStatsChanged(IMOUnit *pUnit, float fHP, float fAmmo1, float fAmmo2) override;
  // get selection ID
  const int GetSelectionGroupID() const override;
};

// //////////////////////////////////////////////////////////// 
#endif // __MOSQUAD_H__