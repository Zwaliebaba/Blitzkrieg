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
    for (auto it = passangers.begin(); it != passangers.end(); ++it) { if (it->pUnit == pUnit) return &(*it); }
    return nullptr;
  }

  //
  void UpdateVisObj();
  //
  CMOSquad();
  ~CMOSquad() override;

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
  //
  bool STDCALL CanSelect() const override { return passangers.empty() ? false : passangers.back().pUnit->CanSelect(); }
  // get status for mission status bar
  void STDCALL GetStatus(struct SMissionStatusObject *pStatus) const override {}
  // get actions, which this object can perform or actions, thi object can be acted with
  void STDCALL GetActions(CUserActions *pActions, EActionsType eActions) const override;
  // common updates
  int STDCALL AIUpdateActions(const struct SAINotifyAction &action, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene, interface IClientAckManager *pAckManager) override { return 0; }
  void STDCALL AIUpdatePlacement(const struct SAINotifyPlacement &placement, const NTimer::STime &currTime, IScene *pScene) override {}
  bool STDCALL AIUpdateRPGStats(const struct SAINotifyRPGStats &stats, IVisObjBuilder *pVOB, IScene *pScene) override { return true; }
  bool STDCALL AIUpdateDiplomacy(const struct SAINotifyDiplomacy &diplomacy) override;
  void STDCALL AIUpdateHit(const struct SAINotifyHitInfo &hit, const NTimer::STime &currTime, IScene *pScene, IVisObjBuilder *pVOB) override {}
  // firing... (from container of by himself)
  void STDCALL AIUpdateShot(const struct SAINotifyBaseShot &shot, const NTimer::STime &currTime, IVisObjBuilder *pVOB, IScene *pScene) override {}
  // visiting
  void STDCALL Visit(IMapObjVisitor *pVisitor) override {}
  // check, is this object selected?
  bool STDCALL IsSelected() const override;
  // change selection state for this object
  void STDCALL Select(ISelector *pSelector, bool bSelect, bool bSelectSuper) override;
  // load unit onboard or unload it
  bool STDCALL Load(interface IMOUnit *pMO, bool bEnter) override;
  //
  void STDCALL UpdatePassangers() override {}
  // get all passangers from container. 
  int STDCALL GetPassangers(IMOUnit **pBuffer, bool bCanSelectOnly) const override;
  // get free places
  int STDCALL GetFreePlaces() const override { return 0; }
  // notify about RPG stats changing fot the single squad member
  void STDCALL NotifyStatsChanged(IMOUnit *pUnit, float fHP, float fAmmo1, float fAmmo2) override;
  // get selection ID
  const int STDCALL GetSelectionGroupID() const override;
};

// //////////////////////////////////////////////////////////// 
#endif // __MOSQUAD_H__