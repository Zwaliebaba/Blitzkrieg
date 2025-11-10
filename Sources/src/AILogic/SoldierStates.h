#ifndef __SOLDIER_STATES_H__
#define __SOLDIER_STATES_H__

#pragma once
// ///////////////////////////////////////////////////////////// 
#include "UnitStates.h"
#include "StatesFactory.h"
#include "CommonStates.h"
#include "DamageToEnemyUpdater.h"
#include "RectTiles.h"
#include "../Common/Actions.h"
// ///////////////////////////////////////////////////////////// 
class CBuilding;
class CEntrenchment;
class CMineStaticObject;
class CMilitaryCar;
class CFormation;

// ///////////////////////////////////////////////////////////// 
class CSoldierStatesFactory : public IStatesFactory
{
  OBJECT_COMPLETE_METHODS(CSoldierStatesFactory);

  static CPtr<CSoldierStatesFactory> pFactory;

public:
  static IStatesFactory *Instance();
  interface IUnitState *ProduceState(class CQueueUnit *pUnit, class CAICommand *pCommand) override;
  interface IUnitState *ProduceRestState(class CQueueUnit *pUnit) override;

  bool CanCommandBeExecuted(class CAICommand *pCommand) override;
  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// ///////////////////////////////////////////////////////////// 
class CSoldierRestState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CSoldierRestState);
  DECLARE_SERIALIZE;

  CAIUnit *pUnit;

  NTimer::STime startTime;
  NTimer::STime nextMove;
  bool bScanned;
  CVec2 guardPoint;
  float fDistToGuardPoint;

public:
  static IUnitState *Instance(class CAIUnit *pUnit);

  CSoldierRestState() : pUnit(nullptr) {}
  CSoldierRestState(class CAIUnit *pUnit);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_REST; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return guardPoint; }

  class CAIUnit *GetTarget() const { return nullptr; }

  // at 1 - some short time for the update to occur, not 0 - because 
  void SetNullLastMoveTime() { nextMove = 1; }
};

// ///////////////////////////////////////////////////////////// 
class CSoldierAttackState : public IUnitAttackingState, public CStandartBehaviour
{
  OBJECT_COMPLETE_METHODS(CSoldierAttackState);
  DECLARE_SERIALIZE;

  enum { SHOOTING_CHECK = 3000, ENEMY_DIR_TOLERANCE = 10000 };

  enum ESoldierAttackStates { ESAS_BRUTE_MOVING, ESAS_MOVING, ESAS_MOVING_TO_SIDE };

  ESoldierAttackStates state;

  NTimer::STime nextShootCheck;
  SVector lastEnemyTile;
  WORD wLastEnemyDir;

  CVec2 lastEnemyCenter;

  class CAIUnit *pUnit;
  CPtr<CAIUnit> pEnemy;

  CPtr<CBasicGun> pGun;
  bool bAim;
  bool bFinish;
  bool bSwarmAttack;
  int nEnemyParty;

  CDamageToEnemyUpdater damageToEnemyUpdater;
  CRndRunUpToEnemy runUpToEnemy;

  //
  bool IsBruteMoving();
  interface IStaticPath *BestSidePath();

  // frontal attack - drive towards a unit and, when you can shoot, shoot
  void AnalyzeBruteMovingPosition();
  // go to the unit to some distance from it, and then enter from the side
  void AnalyzeMovingPosition();
  // enter the unit from the side
  void AnalyzeMovingToSidePosition();

  void FireNow();
  void StopFire();
  void StartAgain();

public:
  static IUnitState *Instance(class CAIUnit *pUnit, class CAIUnit *pEnemy, bool bAim, bool bSwarmAttack);

  CSoldierAttackState() : pUnit(nullptr) {}
  CSoldierAttackState(class CAIUnit *pUnit, class CAIUnit *pEnemy, bool bAim, bool bSwarmAttack);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_ATTACK_UNIT; }
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override;

  class CAIUnit *GetTarget() const { return pEnemy; }

  bool IsAttacksUnit() const override { return true; }
  class CAIUnit *GetTargetUnit() const override { return GetTarget(); }
};

// ///////////////////////////////////////////////////////////// 
class CSoldierMoveToState : public IUnitState, public CFreeFireManager
{
  OBJECT_COMPLETE_METHODS(CSoldierMoveToState);
  DECLARE_SERIALIZE;

  enum { TIME_OF_WAITING = 200 };

  CAIUnit *pUnit;

  NTimer::STime startTime;
  bool bWaiting;
  CVec2 point;
  WORD wDirToPoint;

public:
  static IUnitState *Instance(CAIUnit *pUnit, const CVec2 &point);

  CSoldierMoveToState() : pUnit(nullptr) {}
  CSoldierMoveToState(class CAIUnit *pUnit, const CVec2 &point);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  EUnitStateNames GetName() override { return EUSN_MOVE; }
};

// ///////////////////////////////////////////////////////////// 
class CSoldierTurnToPointState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CSoldierTurnToPointState);
  DECLARE_SERIALIZE;

  CAIUnit *pUnit;

  NTimer::STime lastCheck;
  CVec2 targCenter;

public:
  static IUnitState *Instance(class CAIUnit *pUnit, const CVec2 &targCenter);

  CSoldierTurnToPointState() : pUnit(nullptr) {}
  CSoldierTurnToPointState(class CAIUnit *pUnit, const CVec2 &targCenter);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  EUnitStateNames GetName() override { return EUSN_TURN_TO_POINT; }
};

// ///////////////////////////////////////////////////////////// 
class CSoldierMoveByDirState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CSoldierMoveByDirState);
  DECLARE_SERIALIZE;

  CAIUnit *pUnit;

  //
  void Init(class CAIUnit *pUnit, const CVec2 &vTarget);

public:
  static IUnitState *Instance(class CAIUnit *pUnit, const CVec2 &vTarget);

  CSoldierMoveByDirState() : pUnit(nullptr) {}
  CSoldierMoveByDirState(class CAIUnit *pUnit, const CVec2 &vTarget);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// ///////////////////////////////////////////////////////////// 
class CSoldierEnterState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CSoldierEnterState);
  DECLARE_SERIALIZE;

  enum EEnterStates { EES_START, EES_RUN_UP };

  EEnterStates state;

  CAIUnit *pUnit;

  int nEntrance;
  CPtr<CBuilding> pBuilding;

  int nEfforts;

  //
  bool SetPathForRunUp();

public:
  static IUnitState *Instance(class CAIUnit *pUnit, class CBuilding *pBuilding);

  CSoldierEnterState() : pUnit(nullptr) {}
  CSoldierEnterState(class CAIUnit *pUnit, class CBuilding *pBuilding);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_ENTER; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

// ///////////////////////////////////////////////////////////// 
class CSoldierEnterEntrenchmentState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CSoldierEnterEntrenchmentState);
  DECLARE_SERIALIZE;

  enum EEnterState { EES_START, EES_RUN, EES_FINISHED };

  EEnterState state;

  CAIUnit *pUnit;
  CPtr<CEntrenchment> pEntrenchment;

  //
  bool SetPathForRunIn();
  void EnterToEntrenchment();

public:
  static IUnitState *Instance(class CAIUnit *pUnit, class CEntrenchment *pEntrenchment);

  CSoldierEnterEntrenchmentState() : pUnit(nullptr) {}
  CSoldierEnterEntrenchmentState(class CAIUnit *pUnit, class CEntrenchment *pEntrenchment);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_ENTER_ENTRENCHMENT; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

// ///////////////////////////////////////////////////////////// 
class CSoldierAttackCommonStatObjState : public CCommonAttackCommonStatObjState
{
  OBJECT_COMPLETE_METHODS(CSoldierAttackCommonStatObjState);
  DECLARE_SERIALIZE;

  class CAIUnit *pUnit;

protected:
  class CAIUnit *GetUnit() const override;
  void FireNow() override;

public:
  static IUnitState *Instance(class CAIUnit *pUnit, class CStaticObject *pObj, bool bSwarmAttack);

  CSoldierAttackCommonStatObjState() : pUnit(nullptr) {}
  CSoldierAttackCommonStatObjState(class CAIUnit *pUnit, class CStaticObject *pObj, bool bSwarmAttack);

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override;
  void Segment() override;
};

// ///////////////////////////////////////////////////////////// 
class CSoldierParadeState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CSoldierParadeState);
  DECLARE_SERIALIZE;

  CAIUnit *pUnit;

public:
  static IUnitState *Instance(class CAIUnit *pUnit);

  CSoldierParadeState() : pUnit(nullptr) {}
  CSoldierParadeState(CAIUnit *pUnit);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_PARADE; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  friend class CStaticMembers;
};

// ///////////////////////////////////////////////////////////// 
class CSoldierPlaceMineNowState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CSoldierPlaceMineNowState);
  DECLARE_SERIALIZE;

  CAIUnit *pUnit;

  CVec2 point;
  int/* SMineRPGStats::EType */ nType;
  NTimer::STime beginAnimTime;

public:
  static IUnitState *Instance(class CAIUnit *pUnit, const CVec2 &point, enum SMineRPGStats::EType nType);

  CSoldierPlaceMineNowState() : pUnit(nullptr) {}
  CSoldierPlaceMineNowState(class CAIUnit *pUnit, const CVec2 &point, enum SMineRPGStats::EType nType);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

// ///////////////////////////////////////////////////////////// 
class CSoldierClearMineRadiusState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CSoldierClearMineRadiusState);
  DECLARE_SERIALIZE;

  enum EPutMineStates { EPM_START, EPM_MOVE, EPM_WAITING };

  EPutMineStates eState;

  CAIUnit *pUnit;
  CPtr<CMineStaticObject> pMine;

  CVec2 clearCenter;
  NTimer::STime beginAnimTime;

  //
  bool FindMineToClear();

public:
  static IUnitState *Instance(class CAIUnit *pUnit, const CVec2 &clearCenter);

  CSoldierClearMineRadiusState() : pUnit(nullptr) {}
  CSoldierClearMineRadiusState(class CAIUnit *pUnit, const CVec2 &clearCenter);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return clearCenter; }
};

// ///////////////////////////////////////////////////////////// 
class CSoldierAttackUnitInBuildingState : public CCommonAttackUnitInBuildingState
{
  OBJECT_COMPLETE_METHODS(CSoldierAttackUnitInBuildingState);
  DECLARE_SERIALIZE;

  CAIUnit *pUnit;
  bool bTriedToShootBuilding;

protected:
  class CAIUnit *GetUnit() const override;
  void FireNow() override;

public:
  static IUnitState *Instance(class CAIUnit *pUnit, class CSoldier *pTarget, bool bAim, bool bSwarmAttack);

  CSoldierAttackUnitInBuildingState() : pUnit(nullptr) {}
  CSoldierAttackUnitInBuildingState(class CAIUnit *pUnit, class CSoldier *pTarget, bool bAim, bool bSwarmAttack);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_ATTACK_UNIT_IN_BUILDING; }
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override;
};

// ///////////////////////////////////////////////////////////// 
class CSoldierEnterTransportNowState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CSoldierEnterTransportNowState);
  DECLARE_SERIALIZE;

  enum EEnterTransportStates { EETS_START, EETS_MOVING, EETS_FINISHED };

  EEnterTransportStates eState;

  CAIUnit *pUnit;
  CPtr<CMilitaryCar> pTransport;
  // latest trajectory update (needed for moving vehicles)
  NTimer::STime timeLastTrajectoryUpdate;
  CVec2 vLastTransportCenter;
  WORD wLastTransportDir;

public:
  static IUnitState *Instance(class CAIUnit *pUnit, class CMilitaryCar *pTransport);

  CSoldierEnterTransportNowState() : pUnit(nullptr) {}
  CSoldierEnterTransportNowState(class CAIUnit *pUnit, class CMilitaryCar *pTransport);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

// ///////////////////////////////////////////////////////////// 
// state of soldiers that are dropped from carrier
class CSoldierParaDroppingState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CSoldierParaDroppingState);
  DECLARE_SERIALIZE;

  enum ESoldierParaDroppingState
  {
    ESPDS_FALLING,
    ESPDS_OPEN_PARASHUTE,
    ESPDS_FALLING_W_PARASHUTE,
    ESPDS_CLOSING_PARASHUTE,
    ESPDS_WAIT_FOR_END_UPDATES,
    ESPDS_FINISH_STATE,
  };

  ESoldierParaDroppingState eState;
  ESoldierParaDroppingState eStateToSwitch;


  NTimer::STime timeToCloseParashute, timeToOpenParashute, timeToFallWithParashute;

  CSoldier *pUnit;

public:
  static IUnitState *Instance(class CSoldier *pUnit);

  CSoldierParaDroppingState() : pUnit(nullptr) {}
  CSoldierParaDroppingState(class CSoldier *pUnit);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
  EUnitStateNames GetName() override { return EUSN_PARTROOP; }

};

// ///////////////////////////////////////////////////////////// 
class CSoldierUseSpyglassState : public IUnitState, public CStandartBehaviour
{
  OBJECT_COMPLETE_METHODS(CSoldierUseSpyglassState);
  DECLARE_SERIALIZE;

  CSoldier *pSoldier;
  CVec2 vPoint2Look;

  //
  void SetLookAnimation();

public:
  static IUnitState *Instance(class CSoldier *pSoldier, const CVec2 &point);

  CSoldierUseSpyglassState() : pSoldier(nullptr), vPoint2Look(VNULL2) {}
  CSoldierUseSpyglassState(class CSoldier *pSoldier, const CVec2 &point);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

// ///////////////////////////////////////////////////////////// 
// the unit must attack the platoon. 
class CSoldierAttackFormationState : public IUnitAttackingState
{
  OBJECT_COMPLETE_METHODS(CSoldierAttackFormationState);
  DECLARE_SERIALIZE;

  CAIUnit *pUnit;
  CPtr<CFormation> pTarget;
  bool bSwarmAttack;

public:
  static IUnitState *Instance(class CAIUnit *pUnit, class CFormation *pTarget, bool bSwarmAttack);

  CSoldierAttackFormationState() : pUnit(nullptr) {}
  CSoldierAttackFormationState(class CAIUnit *pUnit, class CFormation *pTarget, bool bSwarmAttack);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override;

  bool IsAttacksUnit() const override { return true; }
  class CAIUnit *GetTargetUnit() const override { return nullptr; }
};

// ///////////////////////////////////////////////////////////// 
class CSoldierIdleState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CSoldierIdleState);
  DECLARE_SERIALIZE;

  CAIUnit *pUnit;

public:
  static IUnitState *Instance(class CAIUnit *pUnit);

  CSoldierIdleState() : pUnit(nullptr) {}
  CSoldierIdleState(class CAIUnit *pUnit);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

// ///////////////////////////////////////////////////////////// 
// to attack aircraft. 
// ground targets towards him. 
// then it switches to it.
class CAviation;

class CSoldierAttackAviationState : public IUnitAttackingState
{
  OBJECT_COMPLETE_METHODS(CSoldierAttackAviationState);
  DECLARE_SERIALIZE;

  enum ESoldierAttackAviationState
  {
    SAAS_ESITMATING,

    SAAS_START_TRASING,// for firing accompanying fire
    SAAS_TRASING,
    SAAS_FIRING,

    SAAS_START_AIMING_TO_PREDICTED_POINT,// for firing defensive fire
    SAAS_AIM_TO_PREDICTED_POINT,
    SAAS_START_FIRE_TO_PREDICTED_POINT,
    SAAS_FIRING_TO_PREDICTED_POINT,

    SAAS_FINISH,
    SAAS_WAIT_FOR_END_OF_BURST,
  };

  class SPredict
  {
    WORD wHor, wVer;
    CVec3 vPt;
    float fRange;
    NTimer::STime timeToFire;

  public:
    SPredict() {}
    SPredict(const CVec3 &pt, float _fRange, NTimer::STime _timeToFire, CAIUnit *pOwner);
    WORD GetHor() const { return wHor; }
    WORD GetVer() const { return wVer; }
    float GetRange() const { return fRange; }
    const CVec3 GetPt() const { return vPt; }
    const NTimer::STime GetFireTime() const { return timeToFire; }
  };

  ESoldierAttackAviationState eState;

  class CAIUnit *pUnit;
  CPtr<CAviation> pPlane;
  bool bAttacking;// true when decided to aim and shoot

  SPredict aimPoint;// aiming point when firing barrage fire
  NTimer::STime timeOfStartBurst;
  NTimer::STime timeLastAimUpdate;

  bool CanFireNow() const;
  void FireNow();
  void StopFire();
  bool IsFinishedFire();
  void Aim();
  bool CalcAimPoint();

public:
  static IUnitState *Instance(class CAIUnit *pUnit, class CAviation *pPlane);

  CSoldierAttackAviationState() : pUnit(nullptr) {}
  CSoldierAttackAviationState(class CAIUnit *pUnit, class CAviation *pPlane);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return bAttacking; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }

  bool IsAttacksUnit() const override { return true; }
  class CAIUnit *GetTargetUnit() const override;
};

// ///////////////////////////////////////////////////////////// 
class CSoldierFireMoraleShellState : public IUnitState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CSoldierFireMoraleShellState);

  class CAIUnit *pUnit;
  int nMoraleGun;
  CVec2 vTarget;

public:
  static IUnitState *Instance(class CAIUnit *pUnit, const class CVec2 &vTarget);

  CSoldierFireMoraleShellState() : pUnit(nullptr) {}
  CSoldierFireMoraleShellState(class CAIUnit *pUnit, const class CVec2 &vTarget);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }
};

// ///////////////////////////////////////////////////////////// 
class CSoldierUseState : public IUnitState
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CSoldierUseState);

  EActionNotify eState;
  class CAIUnit *pUnit;

public:
  static IUnitState *Instance(class CAIUnit *pUnit, const EActionNotify &eState);

  CSoldierUseState() : eState(ACTION_NOTIFY_NONE), pUnit(nullptr) {}
  CSoldierUseState(class CAIUnit *pUnit, const EActionNotify &eState);

  void Segment() override;
  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1.0f, -1.0f); }

  EUnitStateNames GetName() override { return EUSN_USE; }
};

// ///////////////////////////////////////////////////////////// 
#endif // __SOLDIER_STATES_H__