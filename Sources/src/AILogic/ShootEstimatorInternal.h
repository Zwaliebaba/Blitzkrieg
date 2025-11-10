#ifndef __SHOOT_ESTIMATOR_INTERNAL_H__
#define __SHOOT_ESTIMATOR_INTERNAL_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "ShootEstimator.h"
#include "Obstacle.h"
// //////////////////////////////////////////////////////////// 
class CAIUnit;
class CBasicGun;

// //////////////////////////////////////////////////////////// 
class CTankShootEstimator : public IShootEstimator
{
  OBJECT_COMPLETE_METHODS(CTankShootEstimator);
  DECLARE_SERIALIZE;

  class CAIUnit *pOwner;
  CPtr<CAIUnit> pBestUnit;
  CPtr<CBasicGun> pBestGun;
  int nBestGun;
  CPtr<CAIUnit> pCurTarget;
  bool bDamageToCurTargetUpdated;

  float fBestRating;
  DWORD dwForbidden;
  DWORD dwDefaultForbidden;

  CGDBPtr<SUnitBaseRPGStats> pMosinStats;

  // time required to turn pGun on pEnemy
  // const float FindTimeToTurn( CAIUnit *pEnemy, CBasicGun *pGun ) const;
  // select gun for pEnemy
  void ChooseGun(CBasicGun **pBestGun, int *nBestGun, CAIUnit *pEnemy);

  const float GetRating(CAIUnit *pEnemy, CBasicGun *pGun) const;
  const float GetRating(const SUnitBaseRPGStats *pStats, const CVec2 &vCenter, CBasicGun *pGun) const;

public:
  CTankShootEstimator() : pOwner(nullptr) {}
  explicit CTankShootEstimator(class CAIUnit *pOwner);

  void Reset(class CAIUnit *pCurEnemy, bool bDamageUpdated, DWORD dwForbidden) override;
  void AddUnit(class CAIUnit *pEnemy) override;
  class CAIUnit *GetBestUnit() const override;
  class CBasicGun *GetBestGun() const override;
  const int GetNumberOfBestGun() const override;
};

// //////////////////////////////////////////////////////////// 
class CSoldierShootEstimator : public IShootEstimator
{
  OBJECT_COMPLETE_METHODS(CSoldierShootEstimator);
  DECLARE_SERIALIZE;

  static const int N_GOOD_NUMBER_ATTACKING_GRENADES;

  class CAIUnit *pOwner;
  CPtr<CAIUnit> pBestUnit;
  CPtr<CBasicGun> pBestGun;
  int nBestGun;
  CPtr<CAIUnit> pCurTarget;
  bool bDamageToCurTargetUpdated;

  float fBestRating;

  bool bHasGrenades;
  // throw a grenade, do not take into account the general function of selecting a target by rating
  bool bThrowGrenade;

  DWORD dwForbidden;

  CGDBPtr<SUnitBaseRPGStats> pMosinStats;

  // select gun for pEnemy
  void ChooseGun(CBasicGun **pBestGun, int *nBestGun, CAIUnit *pEnemy);

  const float GetRating(CAIUnit *pEnemy, CBasicGun *pGun) const;
  const float GetRating(const SUnitBaseRPGStats *pStats, const CVec2 &vCenter, CBasicGun *pGun) const;

public:
  CSoldierShootEstimator() : pOwner(nullptr) {}
  explicit CSoldierShootEstimator(class CAIUnit *pOwner);

  void Reset(class CAIUnit *pCurEnemy, bool bDamageUpdated, DWORD dwForbidden) override;
  void AddUnit(class CAIUnit *pEnemy) override;
  class CAIUnit *GetBestUnit() const override;
  class CBasicGun *GetBestGun() const override;
  const int GetNumberOfBestGun() const override;
};

// //////////////////////////////////////////////////////////// 
// shooting from onboard shooting points for aircraft
class CPlaneDeffensiveFireShootEstimator : public IShootEstimator
{
  OBJECT_COMPLETE_METHODS(CPlaneDeffensiveFireShootEstimator);
  DECLARE_SERIALIZE;

  class CAIUnit *pOwner;

  CPtr<CAIUnit> pBestUnit;
  CPtr<CAIUnit> pCurTarget;
  CPtr<CBasicGun> pGun;
  bool bDamageToCurTargetUpdated;
  float fBestRating;

  const float CalcTimeToOpenFire(class CAIUnit *pEnemy, CBasicGun *pGun) const;// time to open fire (taking into account the rotation of the weapon and the speed of approaching the enemy)

  const float CalcRating(CAIUnit *pEnemy, CBasicGun *pGun) const;

public:
  CPlaneDeffensiveFireShootEstimator() : pOwner(nullptr) {}
  explicit CPlaneDeffensiveFireShootEstimator(class CAIUnit *pOwner);

  void SetGun(CBasicGun *_pGun);

  void Reset(class CAIUnit *pCurEnemy, bool bDamageUpdated, DWORD dwForbidden) override;// dwFirbidden is ignored
  void AddUnit(class CAIUnit *pEnemy) override;
  class CAIUnit *GetBestUnit() const override;
  class CBasicGun *GetBestGun() const override;
  const int GetNumberOfBestGun() const override;
};

// //////////////////////////////////////////////////////////// 
// for
class CBuilding;

class CPlaneShturmovikShootEstimator : public IShootEstimator
{
  OBJECT_COMPLETE_METHODS(CPlaneShturmovikShootEstimator);
  DECLARE_SERIALIZE;

  struct STargetInfo
  {
    DECLARE_SERIALIZE;

  public:
    CPtr<CAIUnit> pTarget;
    bool bCanTargetShootToPlanes;
    bool bCanAttackerBreakTarget;
    WORD wSpeedDiff;
    WORD wDirToTarget;
    float fRating;
    //
    void Reset()
    {
      bCanTargetShootToPlanes = false;
      bCanAttackerBreakTarget = false;
      fRating = 0;
      wSpeedDiff = 65535;
      pTarget = nullptr;
    }

    STargetInfo() { Reset(); }
  };

  class CAIUnit *pOwner;
  CPtr<CAIUnit> pCurEnemy;
  CVec2 vCenter;

  STargetInfo bestForGuns;
  STargetInfo bestForBombs;
  STargetInfo bestAviation;
  using CBuildings = std::unordered_set<int/* unique id of building */>;
  CBuildings buildings;
  CPtr<CBuilding> pBestBuilding;

  const float CPlaneShturmovikShootEstimator::CalcTimeToOpenFire(CAIUnit *pEnemy) const;
  void CollectTarget(STargetInfo *pInfo, class CAIUnit *pTarget, DWORD dwPossibleGuns);
  const float CalcRating(CAIUnit *pEnemy, DWORD dwPossibleGuns) const;

public:
  CPlaneShturmovikShootEstimator() : pOwner(nullptr) {}
  CPlaneShturmovikShootEstimator(class CAIUnit *pOwner);
  void SetCurCenter(const CVec2 &vNewCenter) { vCenter = vNewCenter; }

  void Reset(class CAIUnit *pCurEnemy, bool bDamageUpdated, DWORD dwForbidden) override;// dwFirbidden is ignored
  void AddUnit(class CAIUnit *pEnemy) override;
  class CAIUnit *GetBestUnit() const override;

  void CalcBestBuilding();
  class CBuilding *GetBestBuilding() const { return pBestBuilding; }

  class CBasicGun *GetBestGun() const override
  {
    NI_ASSERT_T(false, "Wrong call");
    return nullptr;
  }

  const int GetNumberOfBestGun() const override
  {
    NI_ASSERT_T(false, "Wrong call");
    return 0;
  }
};

// //////////////////////////////////////////////////////////// 
// for shooting at obstacles.
class CShootEstimatorForObstacles : public IObstacleEnumerator
{
  class CCommonUnit *pOwner;
  float fCurRating;
  CPtr<IObstacle> pBest;

public:
  CShootEstimatorForObstacles(class CCommonUnit *pOwner) : pOwner(pOwner), fCurRating(0) {}

  bool AddObstacle(IObstacle *pObstacle) override;
  virtual interface IObstacle *GetBest() const;
};

// //////////////////////////////////////////////////////////// 
#endif __SHOOT_ESTIMATOR_INTERNAL_H__