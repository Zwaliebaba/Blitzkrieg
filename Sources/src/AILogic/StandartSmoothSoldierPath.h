#ifndef __STANDART_SMOOTH_SOLDIER_PATH_H__
#define __STANDART_SMOOTH_SOLDIER_PATH_H__

#pragma once

#include "Path.h"

// **********************************************************************
// *CStandartSmoothSoldierPath*
// **********************************************************************

class CStandartSmoothSoldierPath : public ISmoothPath
{
  OBJECT_COMPLETE_METHODS(CStandartSmoothSoldierPath);
  DECLARE_SERIALIZE;

  CPtr<IPath> pPath;
  CPtr<CFormation> pFormation;
  // owner of the path
  interface IBasePathUnit *pUnit;

  CBSpline spline;

  float speed;

  bool bFinished, bNotified, bMinSlowed, bMaxSlowed, bStopped, bWithFormation;

  CVec2 p0, p1, p2, p3;
  CVec2 predPoint;
  float nIter;
  float fRemain;
  int nPoints;

  //
  void AddSmoothTurn();
  int InitSpline();
  // check for locked tiles ahead. 
  // true - if Ok, false - if path had to be recalculated
  bool ValidateCurPath(const CVec2 &center, const CVec2 &newPoint);

  void CutDriveToFormationPath(class CCommonStaticPath *pPath);
  bool CanGoToFormationPos(const CVec2 &newCenter, const CVec2 &vDesPos, const CVec2 &vFormPos);
  bool DriveToFormation(const CVec2 &newCenter, bool bAnyPoint);
  void ValidateCurPathWithFormation(const CVec2 &newCenter);

  const CVec2 GetPointWithFormation(NTimer::STime timeDiff, bool bFirstCall);
  const CVec2 GetPointWithoutFormation(NTimer::STime timeDiff);
  bool CheckTurn(WORD wNewDir);

public:
  CStandartSmoothSoldierPath();
  bool Init(interface IBasePathUnit *pUnit, IPath *pPath, bool bSmoothTurn, bool bCheckTurn = true) override;
  bool InitByFormationPath(class CFormation *pFormation, interface IBasePathUnit *pUnit) override;
  bool Init(interface IMemento *pMemento, interface IBasePathUnit *pUnit) override;

  void SetOwner(interface IBasePathUnit *pUnit) override;
  IBasePathUnit *GetOwner() const override;

  const CVec2 &GetFinishPoint() const override
  {
    if (pPath.IsValid()) return pPath->GetFinishPoint();
    return VNULL2;
  }

  bool IsFinished() const override;

  void Stop() override;

  const CVec3 GetPoint(NTimer::STime timeDiff) override;
  float &GetSpeedLen() override { return speed; }

  void NotifyAboutClosestThreat(interface IBasePathUnit *pUnit, float fDist) override;
  void SlowDown() override;

  bool CanGoBackward() const override { return false; }
  bool CanGoForward() const override { return true; }

  void GetNextTiles(std::list<SVector> *pTiles) override;
  CVec2 GetShift(int nToShift) const override;

  IMemento *GetMemento() const override;

  bool IsWithFormation() const override { return bWithFormation; }
};

#endif // __STANDART_SMOOTH_SOLDIER_PATH_H__