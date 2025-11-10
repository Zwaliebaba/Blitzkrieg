#ifndef __GENERAL_TASKS__
#define __GENERAL_TASKS__

#pragma once

#include "GeneralInternalInterfaces.h"
#include "../Formats/fmtMap.h"
#include "AIHashFuncs.h"
#include "Resistance.h"

// ****
// patch protection.
// ****
class CGeneralTaskToDefendPatch : public IGeneralTask, public IWorkerEnumerator, public IEnemyEnumerator
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CGeneralTaskToDefendPatch);

  IEnemyContainer *pEnemyConatainer;
  class CGeneral *pOwner;

  SAIGeneralParcelInfo patchInfo;
  int nCurReinforcePoint;

  float fSeverity;// current severity of this task
  float fEnemyForce, fFriendlyForce, fFriendlyMobileForce;// 
  float fMaxSeverity;

  bool bFinished;
  bool bWaitForFinish;// wait for finish the task
  NTimer::STime timeLastUpdate;

  CommonUnits infantryInTrenches;// lists of units by type
  CommonUnits infantryFree;
  CommonUnits tanksMobile;
  CommonUnits stationaryUnits;
  CommonUnits enemyForces;

  int nRequestForGunPlaneID;

  void CalcSeverity(bool bEnemyUpdated, bool bFriendlyUpdated);
  void InitTanks(class CCommonUnit *pUnit);
  void InitInfantryInTrenches(class CCommonUnit *pUnit);

public:
  CGeneralTaskToDefendPatch();
  void Init(const struct SAIGeneralParcelInfo &_patchInfo, class CGeneral *pOwner);

  void SetEnemyConatiner(IEnemyContainer *_pEnemyConatainer) override { pEnemyConatainer = _pEnemyConatainer; }

  // ITask
  ETaskName GetName() const override;
  void AskForWorker(ICommander *pManager, float _fMaxSeverity, bool bInit = false) override;
  void ReleaseWorker(ICommander *pManager, float _fMinSeverity) override;
  float GetSeverity() const override;
  bool IsFinished() const override;
  void CancelTask(ICommander *pManager) override;
  void Segment() override;

  // IEnumerator
  bool EnumWorker(class CCommonUnit *pUnit, enum EForceType eType) override;
  bool EvaluateWorker(CCommonUnit *pUnit, enum EForceType eType) const override;

  // IEnemyEnumerator
  bool EnumEnemy(class CAIUnit *pEnemy) override;

};

// ****
// hold reinforcemen
// ****
class CGeneralTaskToHoldReinforcement : public IGeneralTask, public IWorkerEnumerator
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CGeneralTaskToHoldReinforcement);
  CommonUnits tanksFree;
  SAIGeneralParcelInfo patchInfo;
  using UnitsPositions = std::unordered_map<int, CVec2>;
  UnitsPositions unitsPositions;

  float fSeverity;// current severity of this task
  int nCurReinforcePoint;

public:
  CGeneralTaskToHoldReinforcement();
  void Init(const struct SAIGeneralParcelInfo &_patchInfo);
  // empty reinforcement parcell. 
  void Init() { fSeverity = 0; }

  // ITask
  ETaskName GetName() const override { return ETN_HOLD_REINFORCEMENT; }
  void AskForWorker(ICommander *pManager, float fMaxSeverity, bool bInit = false) override;
  void ReleaseWorker(ICommander *pManager, float fMinSeverity) override;
  float GetSeverity() const override;
  bool IsFinished() const override { return false; }
  void CancelTask(ICommander *pManager) override;
  void Segment() override;
  int NeedNBest(const enum EForceType eType) const override { return true; }


  // IEnumerator
  bool EnumWorker(class CCommonUnit *pUnit, enum EForceType eType) override;
  bool EvaluateWorker(CCommonUnit *pUnit, enum EForceType eType) const override;
};

class CGeneralTaskRecaptureStorage : public IGeneralTask, public IWorkerEnumerator
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CGeneralTaskRecaptureStorage);
  CommonUnits tanksFree;
  CVec2 vReinforcePoint;
  float fSeverity;// current severity of this task

  bool bFinished;

public:
  CGeneralTaskRecaptureStorage() {}
  CGeneralTaskRecaptureStorage(const CVec2 &vReinforcePoint);

  // ITask
  ETaskName GetName() const override { return ETN_HOLD_REINFORCEMENT; }
  void AskForWorker(ICommander *pManager, float fMaxSeverity, bool bInit = false) override;
  void ReleaseWorker(ICommander *pManager, float fMinSeverity) override;
  float GetSeverity() const override { return fSeverity; }
  bool IsFinished() const override;
  void CancelTask(ICommander *pManager) override;
  void Segment() override;

  // IEnumerator
  bool EnumWorker(class CCommonUnit *pUnit, enum EForceType eType) override;
  bool EvaluateWorker(CCommonUnit *pUnit, enum EForceType eType) const override;
};

class CGeneralTaskToSwarmToPoint : public IGeneralTask, public IWorkerEnumerator, public IEnemyEnumerator
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CGeneralTaskToSwarmToPoint);

  enum ESwarmState
  {
    ESS_PREPEARING,
    ESS_ATTACKING,
    ESS_REST,
  };

  ESwarmState eState;
  CVec2 vPrepearCenter;

  SResistance curResistanceToAttack;
  float fSeverity;// severity of this task
  float fMaxSeverity;
  float fMinSeverity;
  int nAdditionalIterations;// number of additional iterations to try swarm to
  bool bFinished;
  bool bReleaseWorkers;// this task does not always release workers
  NTimer::STime timeNextCheck;

  IEnemyContainer *pEnemyConatainer;
  class CGeneral *pOwner;

  using CTanks = std::vector<CPtr<CCommonUnit>>;
  CTanks swarmingTanks;
  bool bResistanesBusyByUs;

  CVec2 vTanksPosition;// center of tanks formation
  float fCurDistance;// distance to nearest resistance (during enumeration)

  void ClearResistanceToAcceptNewTask();
  bool IsTimeToRun() const;
  void Run();
  void SendToGroupPoint();

public:
  CGeneralTaskToSwarmToPoint();
  CGeneralTaskToSwarmToPoint(IEnemyContainer *_pEnemyConatainer, class CGeneral *pOwner);

  void SetEnemyConatiner(IEnemyContainer *_pEnemyConatainer) override { pEnemyConatainer = _pEnemyConatainer; }

  // ITask
  ETaskName GetName() const override { return ETN_SWARM_TO_POINT; }
  void AskForWorker(ICommander *pManager, float _fMaxSeverity, bool bInit = false) override;
  void ReleaseWorker(ICommander *pManager, float fMinSeverity) override;
  float GetSeverity() const override;
  bool IsFinished() const override;
  void CancelTask(ICommander *pManager) override;
  void Segment() override;

  // IEnumerator
  bool EnumWorker(class CCommonUnit *pUnit, enum EForceType eType) override;
  bool EvaluateWorker(CCommonUnit *pUnit, enum EForceType eType) const override;
  int NeedNBest(const enum EForceType eType) const override { return 10000; }
  float EvaluateWorkerRating(CCommonUnit *pUnit, enum EForceType eType) const override;

  bool EnumResistances(const struct SResistance &resistance) override;

  bool EnumEnemy(class CAIUnit *pEnemy) override
  {
    NI_ASSERT_T(false, "didn't asked to");
    return false;
  }

  friend class CGeneralSwarmWaitForReady;
};

class CGeneralSwarmWaitForReady : public IGeneralDelayedTask
{
  OBJECT_COMPLETE_METHODS(CGeneralSwarmWaitForReady);
  DECLARE_SERIALIZE;

  CPtr<CGeneralTaskToSwarmToPoint> pGeneralTask;

public:
  CGeneralSwarmWaitForReady() {}

  CGeneralSwarmWaitForReady(class CGeneralTaskToSwarmToPoint *pTask)
    : pGeneralTask(pTask) {}

  bool IsTimeToRun() const override { return pGeneralTask->IsTimeToRun(); }
  void Run() override { pGeneralTask->Run(); }
};


#endif // __GENERAL_TASKS__