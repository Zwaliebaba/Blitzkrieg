#ifndef __IN_BULDING_STATES_H__
#define __IN_BULDING_STATES_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "UnitStates.h"
#include "StatesFactory.h"
#include "Behaviour.h"
#include "CommonStates.h"

// //////////////////////////////////////////////////////////// 
class CInBuildingStatesFactory : public IStatesFactory
{
  OBJECT_COMPLETE_METHODS(CInBuildingStatesFactory);

  static CPtr<CInBuildingStatesFactory> pFactory;

public:
  static IStatesFactory *Instance();

  interface IUnitState *ProduceState(class CQueueUnit *pUnit, class CAICommand *pCommand) override;
  interface IUnitState *ProduceRestState(class CQueueUnit *pUnit) override;
  bool CanCommandBeExecuted(class CAICommand *pCommand) override;

  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// //////////////////////////////////////////////////////////// 
class CSoldierRestInBuildingState : public IUnitState, public CStandartBehaviour
{
  OBJECT_COMPLETE_METHODS(CSoldierRestInBuildingState);
  DECLARE_SERIALIZE;

  class CSoldier *pSoldier;

  NTimer::STime startTime;

public:
  static IUnitState *Instance(class CSoldier *pSoldier, class CBuilding *pBuilding);

  CSoldierRestInBuildingState() : pSoldier(nullptr) {}
  CSoldierRestInBuildingState(class CSoldier *pSoldier);

  void SendUnitTo(class CBuilding *pBuilding);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_REST_IN_BUILDING; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// //////////////////////////////////////////////////////////// 
class CSoldierAttackInBuildingState : public IUnitAttackingState
{
  OBJECT_COMPLETE_METHODS(CSoldierAttackInBuildingState);
  DECLARE_SERIALIZE;

  class CSoldier *pSoldier;
  CPtr<CAIUnit> pEnemy;

  CPtr<CBasicGun> pGun;
  // shooting is over
  bool bFinish;
  // definitely need to aim
  bool bAim;
  int nEnemyParty;

  CDamageToEnemyUpdater damageToEnemyUpdater;

  //
  void AnalyzeCurrentState();

public:
  static IUnitState *Instance(class CSoldier *pSoldier, class CAIUnit *pEnemy);

  CSoldierAttackInBuildingState() : pSoldier(nullptr) {}
  CSoldierAttackInBuildingState(class CSoldier *pSoldier, class CAIUnit *pEnemy);
  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override;

  bool IsAttacksUnit() const override { return true; }
  class CAIUnit *GetTargetUnit() const override;
};

// //////////////////////////////////////////////////////////// 
#endif // __IN_BULDING_STATES_H__