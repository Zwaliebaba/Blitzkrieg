#ifndef __COMMON_STATES_H__
#define __COMMON_STATES_H__

#pragma once

#include "UnitStates.h"
#include "Behaviour.h"
#include "FreeFireManager.h"
#include "RndRunUpToEnemy.h"
#include "DamageToEnemyUpdater.h"

class CAIUnit;
class CSoldier;
class CStaticObject;

class CMechAttackUnitState : public IUnitAttackingState, public CFreeFireManager, public CStandartBehaviour
{
  OBJECT_COMPLETE_METHODS(CMechAttackUnitState);
  DECLARE_SERIALIZE;

  enum { SHOOTING_CHECK = 3000, ENEMY_DIR_TOLERANCE = 10000 };

  enum ESoldierAttackStates { ESAS_BRUTE_MOVING, ESAS_MOVING, ESAS_MOVING_TO_SIDE, ESAS_SIMPLE_FIRING };

  ESoldierAttackStates state;

  NTimer::STime lastShootCheck;
  SVector lastEnemyTile;
  WORD wLastEnemyDir;

  float fProb[4];

  enum EAttackType { EAT_GOOD_PROB, EAT_POOR_PROB };

  EAttackType eAttackType;
  BYTE nBestSide;

  bool bTurningToBest;
  int nBestAngle;
  CVec2 lastEnemyCenter;
  bool bSwarmAttack;
  class CAIUnit *pUnit;
  int nEnemyParty;

  CDamageToEnemyUpdater damageToEnemyUpdater;

  CVec2 vEnemyCenter;
  WORD wEnemyDir;

  //
  bool IsBruteMoving();
  interface IStaticPath *BestSidePath();
  void CalculateProbabilitites();

  void AnalyzeBruteMovingPosition();
  void AnalyzeMovingPosition();
  void AnalyzeMovingToSidePosition();

  void TraceAim();
  // tries to turn his head towards the enemy during the cooldown, if it returned false, then now is not the time to turn
  bool TurnToBestPos();
  // shoot and produce all necessary updates in state
  void FireToEnemy();
  // Is it possible to shoot at an enemy right now without spins?
  bool CanShootToEnemyNow() const;
  // initialize, everything that is needed if pGun is selected for shooting
  void StartStateWithGun(CBasicGun *pGun);
  void FinishState();

protected:
  CPtr<CAIUnit> pEnemy;

  CPtr<CBasicGun> pGun;
  bool bAim;
  bool bFinish;

  //
  virtual void FireNow();
  virtual void StopFire();
  virtual void StartAgain();

public:
  static IUnitState *Instance(class CAIUnit *pOwner, class CAIUnit *pEnemy, bool bAim, bool bSwamAttack);

  CMechAttackUnitState() {}
  CMechAttackUnitState(class CAIUnit *pOwner, class CAIUnit *pEnemy, bool bAim, bool bSwarmAttack);

  void Segment() override;

  class CAIUnit *GetTarget() const { return pEnemy; }
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  bool IsAttacksUnit() const override { return true; }
  class CAIUnit *GetTargetUnit() const override { return GetTarget(); }

  EUnitStateNames GetName() override { return EUSN_ATTACK_UNIT; }
};

class CCommonAttackUnitInBuildingState : public IUnitAttackingState, public CFreeFireManager
{
  DECLARE_SERIALIZE;

  enum EAttackUnitInBuildingStates { EAUBS_START, EAUBS_MOVING_SECTOR, EAUBS_MOVING_UNIT };

  EAttackUnitInBuildingStates eState;

  CDamageToEnemyUpdater damageToEnemyUpdater;
  CRndRunUpToEnemy runUpToEnemy;

  int nSlot;

  //
  bool FindPathToUnit();
  bool FindPathToSector();
  void StartState(class CAIUnit *pOwner);

protected:
  CPtr<CSoldier> pTarget;
  CVec2 targetCenter;
  CPtr<CBasicGun> pGun;
  bool bAim;
  bool bSwarmAttack;

  //
  virtual class CAIUnit *GetUnit() const = 0;
  virtual void FireNow() = 0;
  virtual bool IsInTargetSector() const;

  void FinishState();

public:
  CCommonAttackUnitInBuildingState() {}
  CCommonAttackUnitInBuildingState(class CAIUnit *pOwner, class CSoldier *pTarget, bool bAim, bool bSwarmAttack);

  void Segment() override;

  CSoldier *GetTarget() const { return pTarget; }
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override;

  bool IsAttacksUnit() const override { return true; }
  CAIUnit *GetTargetUnit() const override;
};

class CCommonAttackCommonStatObjState : public IUnitAttackingState, public CFreeFireManager
{
  DECLARE_SERIALIZE;

  //
  bool AttackUnits(class CStaticObject *pObj);
  void AnalyzePosition();
  void AnalyzeShootingObj();

  int nStartObjParty;

protected:
  CPtr<CStaticObject> pObj;
  CPtr<CBasicGun> pGun;

  bool bAim;
  bool bFinish;
  bool bSwarmAttack;

  //
  virtual class CAIUnit *GetUnit() const = 0;
  virtual void FireNow() = 0;
  virtual void StartAgain();
  void FinishState();

public:
  CCommonAttackCommonStatObjState() {}
  CCommonAttackCommonStatObjState(class CAIUnit *pOwner, class CStaticObject *pObj, bool bSwarmAttack);

  void Segment() override;

  EUnitStateNames GetName() override { return EUSN_ATTACK_STAT_OBJECT; }
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override;

  class CStaticObject *GetTarget() const { return pObj; }

  bool IsAttacksUnit() const override { return true; }
  class CAIUnit *GetTargetUnit() const override { return nullptr; }
};

class CCommonRestState : public IUnitState, public CStandartBehaviour
{
  DECLARE_SERIALIZE;

  enum { TIME_OF_CHECK = 2000 };

  CVec2 guardPoint;
  WORD wDir;
  NTimer::STime nextMove;
  NTimer::STime startMoveTime;

  bool bScanned;

  class CCommonUnit *pUnit;

public:
  CCommonRestState() : pUnit(nullptr) {}
  CCommonRestState(const CVec2 &guardPoint, WORD wDir, CCommonUnit *pUnit);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_REST; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return guardPoint; }

  const CVec2 &GetGuardPoint() const { return guardPoint; }
  const WORD GetGuardDir() const { return wDir; }

  // at 1 - some short time for the update to occur, not 0 - because 
  void SetNullLastMoveTime() { nextMove = 1; }
};

class CMechUnitRestState : public CCommonRestState
{
  OBJECT_COMPLETE_METHODS(CMechUnitRestState);
  DECLARE_SERIALIZE;

  class CAIUnit *pUnit;
  bool bFinishWhenCanMove;

public:
  static IUnitState *Instance(class CAIUnit *pUnit, const CVec2 &guardPoint, WORD wDir, bool bFinishWnenCanMove = false);

  CMechUnitRestState() : pUnit(nullptr) {}
  CMechUnitRestState(CAIUnit *pUnit, const CVec2 &guardPoint, WORD wDir, bool bFinishWnenCanMove);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_REST; }
  bool IsAttackingState() const override { return false; }
};

class CCommonAmbushState : public IUnitState, public CStandartBehaviour
{
  OBJECT_COMPLETE_METHODS(CCommonAmbushState);
  DECLARE_SERIALIZE;

  enum { AMBUSH_CHECK = 3000, VISIBLE_CHECK = 500 };

  enum EAmbushStates { EAS_COMMON, EAS_FIRING };

  EAmbushStates eState;

  NTimer::STime startTime;
  NTimer::STime lastCheckTime;
  NTimer::STime lastVisibleCheck;

  CPtr<CBasicGun> pGun;
  CPtr<CAIUnit> pTarget;

  //
  void CommonState();
  void FiringState();

protected:
  CCommonUnit *pUnit;

public:
  static IUnitState *Instance(class CCommonUnit *pUnit);

  CCommonAmbushState() {}
  CCommonAmbushState(class CCommonUnit *pUnit);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_AMBUSH; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  CAIUnit *GetTarget() const { return pTarget; }
};

class CFollowState : public IUnitState, public CStandartBehaviour
{
  OBJECT_COMPLETE_METHODS(CFollowState);
  DECLARE_SERIALIZE;

  enum { CHECK_HEAD = 2000 };

  CCommonUnit *pUnit;
  CPtr<CCommonUnit> pHeadUnit;
  CVec2 lastHeadUnitPos;
  NTimer::STime lastCheck;

public:
  static IUnitState *Instance(class CCommonUnit *pUnit, class CCommonUnit *pHeadUnit);

  CFollowState() {}
  CFollowState(class CCommonUnit *pUnit, class CCommonUnit *pHeadUnit);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

class CCommonSwarmState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CCommonSwarmState);
  DECLARE_SERIALIZE;

  enum { TIME_OF_WAITING = 200 };

  enum ECommonSwarmStates
  {
    ESSS_WAIT,
    ESSS_MOVING,
    ESS_WAIT_UNTIL_TRACK_REPAIR,
  };

  ECommonSwarmStates state;

  CAIUnit *pUnit;

  CVec2 point;
  NTimer::STime startTime;
  bool bContinue;
  WORD wDirToPoint;

public:
  static IUnitState *Instance(class CAIUnit *pUnit, const CVec2 &point, float fContinue);

  CCommonSwarmState() : pUnit(nullptr) {}
  CCommonSwarmState(class CAIUnit *pUnit, const CVec2 &point, float fContinue);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_SWARM; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return point; }
};

class CCommonMoveToGridState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CCommonMoveToGridState);
  DECLARE_SERIALIZE;

  enum EStates
  {
    ES_MOVE,
    ES_WAIT
  };

  CCommonUnit *pUnit;
  CVec2 vPoint;
  CVec2 vDir;
  NTimer::STime startMoveTime;

  EStates eState;

public:
  static IUnitState *Instance(class CCommonUnit *pUnit, const CVec2 &vPoint, const CVec2 &vDir);

  CCommonMoveToGridState() : pUnit(nullptr), eState(ES_WAIT) {}
  CCommonMoveToGridState(class CCommonUnit *pUnit, const CVec2 &vPoint, const CVec2 &vDir);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_MOVE_TO_GRID; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return vPoint; }
};

#endif __COMMON_STATES_H__