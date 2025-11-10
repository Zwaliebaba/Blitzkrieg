#ifndef __OBSTACLEINTERNAL_H__
#define __OBSTACLEINTERNAL_H__

#include "Obstacle.h"

// //////////////////////////////////////////////////////////// 
class CObstacle : public IObstacle
{
  DECLARE_SERIALIZE;
  float fFirePower;

public:
  CObstacle() : fFirePower(0) {}

  void UpdateTakenDamagePower(const float fUpdate) override { fFirePower += fUpdate; }
  const float GetTakenDamagePower() const override { return fFirePower; }

};

// //////////////////////////////////////////////////////////// 
class CStaticObject;

class CObstacleStaticObject : public CObstacle
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CObstacleStaticObject);

  CPtr<CStaticObject> pObj;

public:
  CObstacleStaticObject() {}
  CObstacleStaticObject(class CStaticObject *pObj) : pObj(pObj) {}

  class CBasicGun *ChooseGunToShootToSelf(class CCommonUnit *pUnit, NTimer::STime *pTime) override;
  int GetPlayer() const override;
  float GetHPPercent() const override;
  const CVec2 GetCenter() const override;
  bool IsAlive() const override;
  void IssueUnitAttackCommand(class CCommonUnit *pUnit) override;

  bool CanDeleteByMovingOver(class CAIUnit *pUnit) override;
  interface IUpdatableObj *GetObject() const override;
};

// //////////////////////////////////////////////////////////// 
#endif // __OBSTACLEINTERNAL_H__