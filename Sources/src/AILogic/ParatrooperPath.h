#ifndef __PARATROOPER_PATH_H__
#define __PARATROOPER_PATH_H__

#pragma once

#include "Path.h"

class CParatrooperPath : public ISmoothPath
{
  OBJECT_COMPLETE_METHODS(CParatrooperPath);
  DECLARE_SERIALIZE;

  NTimer::STime lastPathUpdateTime;

  CVec3 vStartPoint;
  CVec3 vFinishPoint;
  CVec2 vFinishPoint2D;

  CVec3 vCurPoint;
  float fSpeedLen;

  CVec3 vHorSpeed;// horizontal speed of parachute

  void FindFreeTile();
  void Init();

public:
  CParatrooperPath() {};
  CParatrooperPath(const CVec3 &startPoint);
  bool IsFinished() const override;
  const CVec3 GetPoint(NTimer::STime timeDiff) override;

  void GetSpeed3(CVec3 *vSpeed) const override;

  const CVec2 &GetFinishPoint() const override { return vFinishPoint2D; }
  // unnecessary functions
  bool Init(interface IBasePathUnit *pUnit, IPath *pPath, bool bSmoothTurn = true, bool bCheckTurn = true) override
  {
    CPtr<IPath> p = pPath;
    return true;
  }

  bool InitByFormationPath(class CFormation *pFormation, interface IBasePathUnit *pUnit) override { return true; }

  bool Init(interface IMemento *pMemento, interface IBasePathUnit *pUnit) override
  {
    CPtr<IMemento> p = pMemento;
    return true;
  }

  void Stop() override {}
  float &GetSpeedLen() override { return fSpeedLen; }
  void NotifyAboutClosestThreat(interface IBasePathUnit *pCollUnit, const float fDist) override {}
  void SlowDown() override {}
  bool CanGoBackward() const override { return false; }
  bool CanGoForward() const override { return true; }
  void GetNextTiles(std::list<SVector> *pTiles) override {}
  CVec2 GetShift(const int nToShift) const override { return CVec2(0, 0); }
  IMemento *GetMemento() const override { return nullptr; }
  float GetCurvatureRadius() const override { return 0.0f; }

  bool IsWithFormation() const override { return true; }

  void SetOwner(interface IBasePathUnit *pUnit) override {}
  IBasePathUnit *GetOwner() const override { return nullptr; }

  static float CalcFallTime(float fZ);
};

#endif // __PARATROOPER_PATH_H__