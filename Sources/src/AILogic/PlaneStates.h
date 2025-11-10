#ifndef __PLANE_STATES_H__
#define __PLANE_STATES_H__

#pragma once

#include "StatesFactory.h"
#include "UnitStates.h"
#include "DamageToEnemyUpdater.h"

class CAviation;
class CFormation;

class CPlaneStatesFactory : public IStatesFactory
{
  OBJECT_COMPLETE_METHODS(CPlaneStatesFactory);

  static CPtr<CPlaneStatesFactory> pFactory;

public:
  static IStatesFactory *Instance();

  bool CanCommandBeExecuted(class CAICommand *pCommand) override;

  interface IUnitState *ProduceState(class CQueueUnit *pUnit, class CAICommand *pCommand) override;
  interface IUnitState *ProduceRestState(class CQueueUnit *pUnit) override;

  // for Saving/Loading of static members
  friend class CStaticMembers;
};

class CPlaneRestState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CPlaneRestState);
  DECLARE_SERIALIZE;

  class CAviation *pPlane;

  CVec3 vertices[3];
  int curVertex;
  float fHeight;

  void InitTriangle(class CAviation *pPlane, const CVec3 &startVertex);

public:
  static IUnitState *Instance(class CAviation *pPlane, float fHeight = -1);

  CPlaneRestState() : pPlane(nullptr) {}
  CPlaneRestState(class CAviation *_pPlane, float fHeight);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;

  EUnitStateNames GetName() override { return EUSN_REST; }
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
};

class CPlaneDeffensiveFireShootEstimator;

// firing from airborne gun mounts
class CPlaneDeffensiveFire
{
  DECLARE_SERIALIZE;

  class CAviation *pOwner;
  NTimer::STime timeLastBSUUpdate;// for the behavior of airborne gun systems

  CPtr<CPlaneDeffensiveFireShootEstimator> pShootEstimator;
  CDamageToEnemyUpdater damageUpdater;

protected:
  // check the enemies and start shooting back
  void AnalyzeBSU();
  CPlaneDeffensiveFire() : pOwner(nullptr), timeLastBSUUpdate(0) {}
  CPlaneDeffensiveFire(class CAviation *pPlane);
};

// moving from point to point
class CPlanePatrolState : public IUnitAttackingState
{
  DECLARE_SERIALIZE;

protected:
  std::vector<CVec2> vPatrolPoints;// set of patrol points
  int nCurPointIndex;// current patrol point
  class CAviation *pPlane;

  const CVec2 &GetPoint() const { return vPatrolPoints[nCurPointIndex % vPatrolPoints.size()]; }
  const void ToNextPoint() { ++nCurPointIndex; }

  void InitPathByCurDir(float fDesiredHeight);

  virtual void ToTakeOffState() = 0;

  CAviation *FindBetterEnemiyPlane(CAviation *pEnemie, float fRadius) const;
  CAviation *FindNewEnemyPlane(float fRadius) const;

public:
  CPlanePatrolState() : nCurPointIndex(0), pPlane(nullptr) {}
  CPlanePatrolState(CAviation *pPlane, const CVec2 &point);
  int GetNPoints() const { return vPatrolPoints.size(); }
  // to add patrol points
  void AddPoint(const CVec2 &vAddPoint);
  void TakeOff() { ToTakeOffState(); }
  void Escape(int /* SUCAviation::AIRCRAFT_TYPE */ nAviaType);

  bool IsAttacksUnit() const override { return false; }
  class CAIUnit *GetTargetUnit() const override { return nullptr; }
  void RegisterPoints(int /* SUCAviation::AIRCRAFT_TYPE */ nPlaneType);
  void UnRegisterPoints();
};

class CPlaneBombState : public CPlanePatrolState, public CPlaneDeffensiveFire
{
  OBJECT_COMPLETE_METHODS(CPlaneBombState);
  DECLARE_SERIALIZE;

  enum ECurBombState
  {
    _WAIT_FOR_TAKEOFF,
    ECBS_ESTIMATE,

    ECBS_GAIN_HEIGHT,
    ECBS_GAIN_DISTANCE,

    ECBS_APPROACH,
    ECBS_WAIT_BOMBPOINT_REACH,
    ECBS_ATTACK_DIVE,
    ECBS_ATTACK_HORISONTAL,

    ECBS_AIM_TO_NEXT_POINT,
    ECBS_AIM_TO_NEXT_POINT_2,
    ECBS_START_ESACPE,
  };

  ECurBombState eState;
  bool bDive;// dive bomber or not
  bool bDiveInProgress;

  float fInitialHeight;
  float fStartAttackDist;

  float fFormerVerticalSpeed;// to determine exit from diving
  bool bExitDive;
  NTimer::STime timeOfStart;// time of start patrolling
protected:
  void ToTakeOffState() override;

public:
  static IUnitState *Instance(CAviation *pPlane, const CVec2 &point);

  CPlaneBombState() {}
  CPlaneBombState(CAviation *pUnit, const CVec2 &point);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override { return GetPoint(); }
  virtual bool IsDiving() const { return bDiveInProgress; }
  EUnitStateNames GetName() override { return EUSN_DIVE_BOMBING; }
};

// for drop paratroopers in specific point
class CPlaneParaDropState : public CPlanePatrolState, public CPlaneDeffensiveFire
{
  OBJECT_COMPLETE_METHODS(CPlaneParaDropState);
  DECLARE_SERIALIZE;

  enum EPlaneParaDropState
  {
    _WAIT_FOR_TAKEOFF,
    PPDS_ESTIMATE,
    PPDS_APPROACHNIG,
    PPDS_PREPARE_TO_DROP,
    PPDS_DROPPING,
    PPDS_TO_NEXT_POINT,
  };

  EPlaneParaDropState eState;

  int nSquadNumber;
  CPtr<CFormation> pSquad;// parachutist platoon
  int nDroppingSoldier;// current soldier to drop

  CVec2 vLastDrop;// the point at which the last parachutist is ejected
  // true if some tiles around drop site are unlocked.
  bool CanDrop(const CVec2 &point);

protected:
  void ToTakeOffState() override;

public:
  static IUnitState *Instance(CAviation *pPlane, const CVec2 &vDestPoint);

  CPlaneParaDropState() {}
  CPlaneParaDropState(CAviation *pPlane, const CVec2 &vDestPoint);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;

  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// state for fighters. 
// 1) fight incoming enemie bombers
class CPlaneFighterPatrolState : public CPlanePatrolState, public CPlaneDeffensiveFire
{
  OBJECT_COMPLETE_METHODS(CPlaneFighterPatrolState);
  DECLARE_SERIALIZE;

  enum ECurFighterOnEnemieState
  {
    _WAIT_FOR_TAKEOFF,
    ECFS_ESCAPE,
    ECFS_GOTO_GUARDPOINT,
    ECFS_GOING_TO_GUARDPOINT,
    ECFS_ENGAGE_TARGET,
    ECFS_AIM_TO_NEXT_POINT,
    ECFS_FIND_ENEMY_OR_NEXT_POINT,
  };

  ECurFighterOnEnemieState eState;

  float fPartolRadius;// patrol radius of this state
  float fPatrolHeight;// height of patrolling
  CPtr<CAviation> pEnemie;// enemie that we attack (plane)

  NTimer::STime timeOfStart;// time of start patrolling
  NTimer::STime timeOfLastPathUpdate;// last update of path
  NTimer::STime timeLastCheck;// last check for ammo availability

  void TryInitPathToEnemie(bool isEnemieNew = false);
  void TryInitPathToPoint(const CVec3 &v, bool isNewPoint = false);

protected:
  void ToTakeOffState() override;
  bool IsEnemyAlive(const class CAviation *pEnemie) const;

public:
  static IUnitState *Instance(CAviation *pPlane, const CVec2 &point);

  CPlaneFighterPatrolState() {}
  CPlaneFighterPatrolState(CAviation *_pPlane, const CVec2 &point);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return true; }
  const CVec2 GetPurposePoint() const override { return CVec2(-1, -1); }

  bool IsAttacksUnit() const override;
  class CAIUnit *GetTargetUnit() const override;

  // for Saving/Loading of static members
  friend class CStaticMembers;
};

class CAIUnit;
class CBuilding;
class CPlaneShturmovikShootEstimator;

class CPlaneShturmovikPatrolState : public CPlanePatrolState, public CPlaneDeffensiveFire
{
  OBJECT_COMPLETE_METHODS(CPlaneShturmovikPatrolState);
  DECLARE_SERIALIZE;

  // to determine whether to throw bombs
  class CBombEstimator
  {
    bool bFire;
    int nMechUnits, nInfantry;
    float fDamage;// damage from one bomb
    float fDisp;// the circle in which the units should be
    const CVec2 vCenter;
    const float fFlyTime;

  public:
    CBombEstimator(class CAviation *pAttacker,
                   float fDamage,
                   const CVec2 &vCenter,
                   float fDisp,
                   float fFlyTime);

    bool Collect(class CAIUnit *pTry);
    bool NeedDrop() const { return bFire; }
  };

  enum EPlaneShturmovikPatrolState
  {
    _WAIT_FOR_TAKEOFF,
    PSPS_ESCAPE,
    PSPS_GOTO_GUARDPOINT,
    PSPS_GOING_TO_GUARDPOINT,
    PSPS_AIM_TO_NEXT_POINT,
    PSPS_FIND_ENEMY_OR_NEXT_POINT,

    PSPS_APPROACH_TARGET,
    PSPS_APPROACHING_TARGET,
    PSPS_ENGAGING_TARGET,
    PSPS_APPROACHING_TARGET_TOWARS_IT,
    PSPS_FIRE_TO_WORLD,
    PSPS_GAIN_HEIGHT,
  };

  class CEnemyContainer
  {
    DECLARE_SERIALIZE;
    CAIUnit *pOwner;

    float fTakenDamage;
    CPtr<CAIUnit> pEnemy;
    CPtr<CBuilding> pBuilding;

    CEnemyContainer(const CEnemyContainer &r);
    CEnemyContainer operator=(const CEnemyContainer &r);

  public:
    CEnemyContainer() : pOwner(nullptr), fTakenDamage(0.0f) {}
    CEnemyContainer(CAIUnit *pOwner) : pOwner(pOwner), fTakenDamage(0.0f) {}

    CVec2 GetCenter() const;
    float GetZ() const;
    bool CanShootToTarget(class CBasicGun *pGun) const;
    void StartBurst(class CBasicGun *pGun);

    void SetEnemy(CAIUnit *pNewEnemy);
    void SetEnemy(CBuilding *pBuilding);

    bool IsValidBuilding() const;
    bool IsValidUnit() const;
    bool IsValid() const;// is alive and is valid

    CAIUnit *GetEnemy();
    CBuilding *GetBuilding();
  };

  EPlaneShturmovikPatrolState eState;
  CVec3 vCurTargetPoint;// the point where the plane is heading

  CPtr<CPlaneShturmovikShootEstimator> pShootEstimator;

  class CAviation *pPlane;

  CEnemyContainer enemie;// enemie that we attack (ground target)

  float fPatrolHeight;

  NTimer::STime timeOfStart;// time of start patrolling
  NTimer::STime timeOfLastPathUpdate;// last update of path
  NTimer::STime timeLastCheck;// checking for cartridges

  float fStartAttackDist;// dive start distance
  float fFinishAttckDist;// attack release distance
  float fTurnRadius;// attack aircraft turning radius
  enum EGunplaneCalledAs eCalledAs;// 

  void TryInitPathToEnemie();
  void TryInitPathToPoint(const CVec3 &v, bool isNewPoint = false);
  bool FindNewEnemie();

  // choose the best
  CAIUnit *FindEnemyInPossibleDiveSector();
  CAIUnit *FindEnemyInFiringSector();

  void TryBurstAllGuns();
  void TryDropBombs();
  void TryBurstAllGunsToPoints();
  bool IsTargetBehind(const CVec2 &vTarget) const;

protected:
  void ToTakeOffState() override;

public:
  static IUnitState *Instance(CAviation *pPlane, const CVec2 &point, int eCalledAs);

  CPlaneShturmovikPatrolState() : pPlane(nullptr) {}
  CPlaneShturmovikPatrolState(CAviation *_pPlane, const CVec2 &point, int eCalledAs);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return GetPoint(); }
};

class CPlaneScoutState : public CPlanePatrolState, public CPlaneDeffensiveFire
{
  OBJECT_COMPLETE_METHODS(CPlaneScoutState);
  DECLARE_SERIALIZE;

  enum EPlaneScoutState
  {
    _WAIT_FOR_TAKEOFF,
    EPSS_GOTO_GUARDPOINT,
    EPSS_GOING_TO_GUARDPOINT,
    EPSS_AIM_TO_NEXT_POINT,
    EPSS_ESCAPE,
  };

  EPlaneScoutState eState;

  float fPatrolHeight;// height of patrolling
  NTimer::STime timeOfStart;

protected:
  void ToTakeOffState() override;

public:
  static IUnitState *Instance(CAviation *pPlane, const CVec2 &point);

  CPlaneScoutState() {}
  CPlaneScoutState(CAviation *_pPlane, const CVec2 &point);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override { return GetPoint(); }
};

// ACTION_MOVE_PLANE_LEAVE
// plane must leave the map through the some point
class CPlaneLeaveState : public IUnitState, public CPlaneDeffensiveFire
{

  OBJECT_COMPLETE_METHODS(CPlaneLeaveState);
  DECLARE_SERIALIZE;

  class CAviation *pPlane;
  CPtr<IUnitState> pMoveToExitPoint;
  int nAviaType;

  enum EPlaneLeaveState
  {
    EPLS_STARTING,
    EPLS_IN_ROUTE,
  };

  EPlaneLeaveState eState;

public:
  static IUnitState *Instance(CAviation *pPlane, int nAviaType);

  CPlaneLeaveState() : pPlane(nullptr) {}
  CPlaneLeaveState(CAviation *pPlane, int nAviaType);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
  EUnitStateNames GetName() override { return EUSN_REST; }

  // for Saving/Loading of static members
  friend class CStaticMembers;
};

// death of the plane.
class CPlaneFlyDeadState : public IUnitState
{
  OBJECT_COMPLETE_METHODS(CPlaneFlyDeadState);
  DECLARE_SERIALIZE;

  enum EPlaneDeadState
  {
    EPDS_START_DIVE,
    EPDS_DIVE,
    EPDS_ESTIMATE,
    EPDS_WAIT_FINISH_PATH,
  };

  EPlaneDeadState eState;

  class CDeadZone
  {
    float fMaxX, fMinX;
    float fMinY, fMaxY;

  public:
    CDeadZone() : fMaxX(0.0f), fMinX(0.0f), fMinY(0.0f), fMaxY(0.0f) {}
    void Init();
    bool IsInZone(const CVec2 &vPoint);
    void AdjustEscapePoint(CVec2 *pPoint);
  };

  CDeadZone deadZone;// out of this rect planes die

  class CAviation *pPlane;
  float fHeight;
  bool bFatality;
  bool bExplodeInstantly;// if false plane will explode after finish dive.
  NTimer::STime timeStart;// start death time

  void InitPathToNearestPoint();
  float CalcPath(WORD wCurDir, byte nDesiredDir, bool bRight, float fTurnRadius, CVec2 *vDestPoint);

public:
  static IUnitState *Instance(CAviation *pPlane);

  CPlaneFlyDeadState() : pPlane(nullptr) {}
  CPlaneFlyDeadState(CAviation *_pPlane);

  void Segment() override;

  ETryStateInterruptResult TryInterruptState(class CAICommand *pCommand) override;
  bool IsAttackingState() const override { return false; }
  const CVec2 GetPurposePoint() const override;
  EUnitStateNames GetName() override { return EUSN_FLY_DEAD; }
};

#endif // __PLANE_STATES_H__