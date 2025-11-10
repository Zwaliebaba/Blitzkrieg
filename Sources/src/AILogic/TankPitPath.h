#ifndef __TANK_PIT_PATH_H__
#define __TANK_PIT_PATH_H__

#pragma once

#include "Path.h"

// a specific path for a unit to take when leaving the TankPit.
// the tank does not turn when walking along this path. 
// stay.
class CTankPitPath : public ISmoothPath
{
  OBJECT_COMPLETE_METHODS(CTankPitPath);
  DECLARE_SERIALIZE;

  CVec2 vCurPoint;
  CVec2 vEndPoint;
  float fSpeedLen;

  interface IBasePathUnit *pUnit;

public:
  CTankPitPath() : pUnit(nullptr) {}
  CTankPitPath(IBasePathUnit *pUnit, const class CVec2 &vStartPoint, const class CVec2 &vEndPoint);
  bool IsFinished() const override;
  const CVec3 GetPoint(NTimer::STime timeDiff) override;

  const CVec2 &GetFinishPoint() const override { return vEndPoint; }
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
  CVec2 GetShift(const int nToShift) const override { return CVec2(0, 0); };
  IMemento *GetMemento() const override { return nullptr; }
  float GetCurvatureRadius() const override { return 0.0f; }

  bool IsWithFormation() const override { return false; }

  void SetOwner(interface IBasePathUnit *_pUnit) override { pUnit = _pUnit; }
  IBasePathUnit *GetOwner() const override { return pUnit; }
};

#endif // __TANK_PIT_PATH_H__