#ifndef __OBSTACLE_H__
#define __OBSTACLE_H__

// //////////////////////////////////////////////////////////// 
// wrapper pattern
// a shell for all the obstacles that need to be destroyed on your way
interface IObstacle : IRefCount
{
  // in order not to shoot at 1 obstacle
  virtual void UpdateTakenDamagePower(float fUpdate) = 0;
  virtual const float GetTakenDamagePower() const = 0;

  // selects the gun from which the unit will better destroy this obstacle
  virtual class CBasicGun *ChooseGunToShootToSelf(class CCommonUnit *pUnit, NTimer::STime *pTime) = 0;

  virtual int GetPlayer() const = 0;
  virtual float GetHPPercent() const = 0;
  virtual const CVec2 GetCenter() const = 0;
  virtual bool IsAlive() const = 0;

  // places a command to attack this object at the beginning of the unit’s queue
  virtual void IssueUnitAttackCommand(class CCommonUnit *pUnit) = 0;

  // can a unit destroy an object simply by driving over it.
  virtual bool CanDeleteByMovingOver(class CAIUnit *pUnit) = 0;
  virtual interface IUpdatableObj *GetObject() const = 0;
};

// //////////////////////////////////////////////////////////// 
interface IObstacleEnumerator
{
  // true if enough objects enumerated.
  virtual bool AddObstacle(IObstacle *pObstacle) = 0;
};

// //////////////////////////////////////////////////////////// 
#endif // __OBSTACLE_H__