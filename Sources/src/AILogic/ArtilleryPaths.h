#ifndef __ARTILLERY_PATHS_H__
#define __ARTILLERY_PATHS_H__

#pragma once

#include "Path.h"

// a specific path for artillerymen - straight through everything.
// this path is activated only when the gunners are already at the cannon.
class CArtilleryCrewPath : public ISmoothPath
{
  OBJECT_COMPLETE_METHODS(CArtilleryCrewPath);
  DECLARE_SERIALIZE;

  interface IBasePathUnit *pUnit;

  CVec2 vCurPoint;
  CVec2 vEndPoint;
  float fSpeedLen;
  bool bSelfSpeed;
  bool bNotInitialized;
  CVec3 vSpeed3;

public:
  CArtilleryCrewPath()
    : pUnit(nullptr), vCurPoint(VNULL2), vEndPoint(VNULL2), fSpeedLen(0.0f), bSelfSpeed(false), bNotInitialized(true), vSpeed3(VNULL3) {}

  CArtilleryCrewPath(interface IBasePathUnit *pUnit, const CVec2 &vStartPoint, const CVec2 &vEndPoint, float fMaxSpeed = 0.0f);

  void SetParams(const CVec2 &vEndPoint, float fMaxSpeed);
  void SetParams(const CVec2 &_vEndPoint, float fMaxSpeed, const CVec2 &_vSpeed2);

  bool IsFinished() const override;
  const CVec3 GetPoint(NTimer::STime timeDiff) override;

  const CVec2 &GetFinishPoint() const override { return vEndPoint; }
  // unnecessary functions
  bool Init(interface IBasePathUnit *pUnit, IPath *pPath, bool bSmoothTurn = true, bool bCheckTurn = true) override;
  bool InitByFormationPath(class CFormation *pFormation, interface IBasePathUnit *pUnit) override { return true; }
  bool Init(interface IMemento *pMemento, interface IBasePathUnit *pUnit) override;
  void Stop() override {}
  float &GetSpeedLen() override { return fSpeedLen; }
  void NotifyAboutClosestThreat(interface IBasePathUnit *pCollUnit, const float fDist) override {}
  void SlowDown() override {}
  bool CanGoBackward() const override { return false; }
  bool CanGoForward() const override { return true; }
  void GetNextTiles(std::list<SVector> *pTiles) override {}
  CVec2 GetShift(const int nToShift) const override { return VNULL2; };
  IMemento *GetMemento() const override { return nullptr; }
  float GetCurvatureRadius() const override { return 0.0f; }
  bool IsWithFormation() const override { return true; }
  // virtual void GetSpeed3( CVec3 *pSpeed ​​) const ;

  void SetOwner(interface IBasePathUnit *_pUnit) override { pUnit = _pUnit; }
  IBasePathUnit *GetOwner() const override { return pUnit; }
};

// path for towed artillery
class CArtilleryBeingTowedPath : public ISmoothPath
{
  OBJECT_COMPLETE_METHODS(CArtilleryBeingTowedPath);
  DECLARE_SERIALIZE;

  float fSpeedLen;
  CVec3 vCurPoint;
  CVec2 vCurPoint2D;
  CVec2 vSpeed;

public:
  CArtilleryBeingTowedPath() : fSpeedLen(0.0f), vCurPoint(VNULL3), vCurPoint2D(VNULL2), vSpeed(VNULL2) {}
  CArtilleryBeingTowedPath(float fSpeedLen, const CVec2 &vCurPoint, const CVec2 &vSpeed);
  bool Init(float fSpeedLen, const class CVec2 &vCurPoint, const CVec2 &vSpeed);

  const CVec2 &GetFinishPoint() const override { return vCurPoint2D; }
  bool IsFinished() const override { return false; }
  const CVec3 GetPoint(NTimer::STime timeDiff) override { return vCurPoint; }

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
  bool CanGoBackward() const override { return true; }
  bool CanGoForward() const override { return true; }
  void GetNextTiles(std::list<SVector> *pTiles) override {}
  CVec2 GetShift(const int nToShift) const override { return VNULL2; };
  IMemento *GetMemento() const override { return nullptr; }
  float GetCurvatureRadius() const override { return 0.0f; }

  bool IsWithFormation() const override { return false; }

  void GetSpeed3(CVec3 *pvSpeed) const override { *pvSpeed = CVec3(vSpeed, 0.0f); }

  void SetOwner(interface IBasePathUnit *pUnit) override {}
  IBasePathUnit *GetOwner() const override { return nullptr; }
};

#endif // __ARTILLERY_PATHS_H__