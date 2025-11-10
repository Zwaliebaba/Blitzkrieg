#ifndef __IN_ENTRENCHMENT_STATES_H__
#define __IN_ENTRENCHMENT_STATES_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "UnitStates.h"
#include "StatesFactory.h"
#include "Behaviour.h"
#include "CommonStates.h"
// //////////////////////////////////////////////////////////// 
class CAIUnit;

// //////////////////////////////////////////////////////////// 
class CInEntrenchmentStatesFactory : public IStatesFactory
{
  OBJECT_COMPLETE_METHODS(CInEntrenchmentStatesFactory);

  static CPtr<CInEntrenchmentStatesFactory> pFactory;

public:
  static IStatesFactory *Instance();
  interface IUnitState *ProduceState(class CQueueUnit *pUnit, class CAICommand *pCommand) override;
  interface IUnitState *ProduceRestState(class CQueueUnit *pUnit) override;
  bool CanCommandBeExecuted(class CAICommand *pCommand) override;

  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// //////////////////////////////////////////////////////////// 
class CSoldierRestInEntrenchmentState : public IUnitState, public CStandartBehaviour
{
  OBJECT_COMPLETE_METHODS(CSoldierRestInEntrenchmentState);
  DECLARE_SERIALIZE;

  class CSoldier *pSoldier;
  NTimer::STime startTime;

public:
  static IUnitState *Instance(class CSoldier *pSoldier, class CEntrenchment *pEntrenchment);

  CSoldierRestInEntrenchmentState() : pSoldier(nullptr) {}
  CSoldierRestInEntrenchmentState(class CSoldier *pSoldier);

  void SetUnitTo(class CEntrenchment *pEntrenchment);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_REST_ENTRENCHMENT; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// //////////////////////////////////////////////////////////// 
class CSoldierAttackInEtrenchState : public IUnitAttackingState
{
  OBJECT_COMPLETE_METHODS(CSoldierAttackInEtrenchState);
  DECLARE_SERIALIZE;

  class CSoldier *pSoldier;
  CPtr<CAIUnit> pEnemy;

  CPtr<CBasicGun> pGun;
  bool bFinish;
  bool bAim;
  bool bSwarmAttack;
  int nEnemyParty;

  CDamageToEnemyUpdater damageToEnemyUpdater;

  //
  void AnalyzeCurrentState();
  void FinishState();

public:
  static IUnitState *Instance(class CSoldier *pSoldier, class CAIUnit *pEnemy, bool bSwarmAttack);

  CSoldierAttackInEtrenchState() : pSoldier(nullptr) {}
  CSoldierAttackInEtrenchState(class CSoldier *pSoldier, class CAIUnit *pEnemy, bool bSwarmAttack);
  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override;

  bool IsAttacksUnit() const override { return true; }
  class CAIUnit *GetTargetUnit() const override;
};

// //////////////////////////////////////////////////////////// 
#endif // __IN_ENTRENCHMENT_STATES_H__