#ifndef __POINT_CHECKING_H__
#define __POINT_CHECKING_H__

#pragma once

#include "PathFinder.h"

class CAttackPointChecking : public IPointChecking
{
  OBJECT_COMPLETE_METHODS(CAttackPointChecking);

  float fRangeMin, fRangeMax;
  SVector targetTile;

public:
  CAttackPointChecking() {}

  CAttackPointChecking(const float _fRangeMin, const float _fRangeMax, const SVector &_targetTile)
    : fRangeMin(_fRangeMin), fRangeMax(_fRangeMax), targetTile(_targetTile) {}

  bool IsGoodTile(const SVector &curTile) const override;
};

class CAttackSideChecking : public IPointChecking
{
  OBJECT_COMPLETE_METHODS(CAttackSideChecking);

  WORD wAttackDir, wHalfAngle;
  float fRangeMin, fRangeMax;
  SVector targetTile;

public:
  CAttackSideChecking() {}

  CAttackSideChecking(float _fRangeMin, const float _fRangeMax, const SVector _targetTile, const WORD _wAttackDir, const WORD _wHalfAngle)
    : wAttackDir(_wAttackDir), wHalfAngle(_wHalfAngle * 4 / 5), fRangeMin(_fRangeMin), fRangeMax(_fRangeMax), targetTile(_targetTile) {}

  bool IsGoodTile(const SVector &curTile) const override;
};

class CGoToDistance : public IPointChecking
{
  OBJECT_COMPLETE_METHODS(CGoToDistance);

  float tileDistance2;
  SVector targetTile;

public:
  CGoToDistance() {}
  // the distance is specified in tiles
  CGoToDistance(const float tileDistance, const SVector &_targetTile)
    : tileDistance2(sqr(tileDistance)), targetTile(_targetTile) {}

  bool IsGoodTile(const SVector &curTile) const override;
};

class CAttackStObjectChecking : public IPointChecking
{
  OBJECT_COMPLETE_METHODS(CAttackStObjectChecking);

  float fRangeMin, fRangeMax;
  SVector targetTile;

public:
  CAttackStObjectChecking() {}
  CAttackStObjectChecking(float fRangeMin, float fRangeMax, class CStaticObject *pObj, CVec2 vAttackingUnitCenter);

  bool IsGoodTile(const SVector &curTile) const override;
};

#endif // __POINT_CHECKING_H__