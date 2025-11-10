#ifndef __SHOOT_ESTIMATOR_H__
#define __SHOOT_ESTIMATOR_H__

#pragma once
// //////////////////////////////////////////////////////////// 
interface IShootEstimator : IRefCount
{
  virtual void Reset(class CAIUnit *pCurEnemy, bool bDamageUpdated, DWORD dwForbidden) = 0;
  virtual void AddUnit(class CAIUnit *pUnit) = 0;
  virtual class CAIUnit *GetBestUnit() const = 0;
  virtual class CBasicGun *GetBestGun() const = 0;
  virtual const int GetNumberOfBestGun() const = 0;
};

// //////////////////////////////////////////////////////////// 
#endif __SHOOT_ESTIMATOR_H__