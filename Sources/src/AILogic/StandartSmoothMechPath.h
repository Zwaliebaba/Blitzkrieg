#ifndef __STANDART_SMOOTH_MECH_PATH__
#define __STANDART_SMOOTH_MECH_PATH__

#pragma once

#include "Path.h"

//*******************************************************************
//*										CStandartSmoothMechPath												*
//*******************************************************************

class CStandartSmoothMechPath : public ISmoothPath
{
  OBJECT_COMPLETE_METHODS(CStandartSmoothMechPath);
  DECLARE_SERIALIZE;

  CPtr<IPath> pPath;
  // владелец пути
  interface IBasePathUnit *pUnit;

  CBSpline spline;

  float speed;

  bool bFinished, bNotified, bMinSlowed, bMaxSlowed, bStopped, bSmoothTurn;

  CVec2 p0, p1, p2, p3;
  CVec2 predPoint;
  float nIter;
  float fRemain;
  int nPoints;
  CVec2 vLastValidatedPoint;

  bool bCanGoForward, bCanGoBackward;
  NTimer::STime lastCheckToRightTurn;

  //
  void AddSmoothTurn();
  int InitSpline();
  // проверить на наличие впереди залоканных тайлов. ≈сли есть, то пересчитать путь
  // true - if Ok, false - if path had to be recalculated
  bool ValidateCurPath(const CVec2 &center, const CVec2 &newPoint);

  const CVec2 GetPointWithoutFormation(NTimer::STime timeDiff);
  bool CheckTurn(WORD wNewDir);

public:
  CStandartSmoothMechPath();
  bool Init(interface IBasePathUnit *pUnit, IPath *pPath, bool bSmoothTurn, bool bCheckTurn = true) override;
  bool InitByFormationPath(class CFormation *pFormation, interface IBasePathUnit *pUnit) override { return true; }
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

  bool CanGoBackward() const override;
  bool CanGoForward() const override { return bCanGoForward; }

  void GetNextTiles(std::list<SVector> *pTiles) override;
  CVec2 GetShift(int nToShift) const override;

  IMemento *GetMemento() const override;

  bool IsWithFormation() const override { return false; }
};

#endif //__STANDART_SMOOTH_MECH_PATH__