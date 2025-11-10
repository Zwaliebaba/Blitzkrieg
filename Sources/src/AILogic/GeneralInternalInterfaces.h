#ifndef __GENERAL_INTERNAL_INTERFACES__
#define __GENERAL_INTERNAL_INTERFACES__

class CFormation;
class CAIUnit;
class CCommonUnit;
using Infantry = std::list<CPtr<CFormation>>;
using MechUnits = std::list<CPtr<CAIUnit>>;
using CommonUnits = std::list<CPtr<CCommonUnit>>;

// //////////////////////////////////////////////////////////// 
enum ETaskName
{
  ETN_DEFEND_PATCH,
  ETN_HOLD_REINFORCEMENT,
  ENT_DEFEND_ESTORAGE,
  ETN_INTENDANT,
  ETN_RESUPPLYCELL,
  ETN_SWARM_TO_POINT,
};

// //////////////////////////////////////////////////////////// 
// for defining type of force type
enum EForceType
{

  _FT_AIR_BEGIN = 0,
  FT_AIR_GUNPLANE = _FT_AIR_BEGIN,
  FT_AIR_SCOUT = 1,
  FT_AIR_BOMBER = 2,
  _FT_AIR_END = FT_AIR_BOMBER,


  FT_INFANTRY_IN_TRENCHES,
  FT_FREE_INFANTRY,

  FT_MOBILE_TANKS,// mobile reinforcement
  FT_SWARMING_TANKS,// tanks that are described to attack group
  FT_STATIONARY_MECH_UNITS,// You're from defense
  FT_RECAPTURE_STORAGE,

  FT_TRUCK_REPAIR_BUILDING,// truck that is able to repair buildings
  FT_TRUCK_RESUPPLY,
};

// //////////////////////////////////////////////////////////// 
// enemies are received through this interface
interface IEnemyEnumerator
{
  // returns whether enemies should still be offered
  virtual bool EnumEnemy(class CAIUnit *pEnemy) = 0;
  virtual bool EnumResistances(const struct SResistance &resistance) { return false; }
};

// //////////////////////////////////////////////////////////// 
// through this - they issue
interface IEnemyContainer
{
  virtual void GiveEnemies(IEnemyEnumerator *pEnumerator) = 0;
  virtual void GiveResistances(IEnemyEnumerator *pEnmumerator) {}
  virtual void AddResistance(const CVec2 &vCenter, float fRadius) = 0;
  virtual void RemoveResistance(const CVec2 &vCenter) = 0;
};

// //////////////////////////////////////////////////////////// 
// ****
// common interface for validating and sorting workers
// ****
interface IWorkerEnumerator
{
  // return FALSE if enumerator already has enough workers
  virtual bool EnumWorker(class CCommonUnit *pUnit, enum EForceType eType) = 0;

  // return the avalability of worker for the specific work ( should be fast )
  virtual bool EvaluateWorker(CCommonUnit *pUnit, enum EForceType eType) const = 0;

  // if enumerator needs only best units of this kind
  virtual int NeedNBest(const enum EForceType eType) const { return 0; }

  // return presize avalability of worker for enumerator
  virtual float EvaluateWorkerRating(CCommonUnit *pUnit, const enum EForceType eType) const { return 1.0f; }
};

// //////////////////////////////////////////////////////////// 
// ****
// common interface for workers container
// ****
interface ICommander : IRefCount
{
  // average complexity of tasks performed by this manager
  virtual float GetMeanSeverity() const = 0;
  // issues workers from a given group, starting¤ with the best by numerator
  virtual void EnumWorkers(EForceType eType, IWorkerEnumerator *pEnumerator) = 0;
  // takes the employee back
  virtual void Give(CCommonUnit *pWorker) = 0;
  virtual void Segment() = 0;

  // give a tip to artillery or aviation or ask for resupply(repair,heal)(cancel existing)
  virtual int /* request ID */RequestForSupport(const CVec2 &vSupportCenter, enum EForceType eType) { return 0; }
  virtual void CancelRequest(int nRequestID, enum EForceType eType) {}
};

// ****
// common interface for task
// ****
interface IGeneralTask : IRefCount
{
  // to determine the task name
  virtual ETaskName GetName() const = 0;
  // so that the task can request workers, but in such a way that the total strength of workers
  // did not exceed the specified
  virtual void AskForWorker(ICommander *pManager, float fMaxSeverity, bool bInit = false) = 0;
  // so that the task can return workers, but so that the total does not become
  // less than specified
  virtual void ReleaseWorker(ICommander *pManager, float fMinSeverity) = 0;
  // tasks are sorted by this parameter¤
  // positive¤ seriousness - everything is fine, negative¤ - situation¤ bad¤
  virtual float GetSeverity() const = 0;

  // task completed
  virtual bool IsFinished() const = 0;

  // force the task to end, return all workers
  virtual void CancelTask(ICommander *pManager) = 0;

  virtual void Segment() = 0;

  virtual void SetEnemyConatiner(IEnemyContainer *_pEnemyConatainer) {}
};

// //////////////////////////////////////////////////////////// 
interface IGeneralDelayedTask : IRefCount
{
  virtual bool IsTimeToRun() const = 0;
  virtual void Run() = 0;
};

// //////////////////////////////////////////////////////////// 
#endif // __GENERAL_INTERNAL_INTERFACES__