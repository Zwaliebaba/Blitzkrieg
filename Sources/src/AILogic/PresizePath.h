#ifndef __PRESIZE_PATH_H__
#define __PRESIZE_PATH_H__

#pragma once

#include "Path.h"

interface IPath;

class CPresizePath : public ISmoothPath
{
  OBJECT_COMPLETE_METHODS(CPresizePath);
  DECLARE_SERIALIZE;

  enum EPresizePathState
  {
    EPPS_WAIT_FOR_INIT,
    EPPS_APPROACH_BY_STANDART,
    EPPS_TURN_TO_DESIRED_POINT,
    EPPS_APPROACH_DESIRED_POINT,
    EPPS_TURN_TO_DESIRED_DIR,
    EPPS_FINISHED,
  };

  EPresizePathState eState;
  CVec2 vEndPoint;
  CVec2 vEndDir;
  WORD wDesiredDir;

  // CRAP{ until we write the normal path. 
  CPtr<ISmoothPath> pPathStandart;
  CPtr<ISmoothPath> pPathCheat;
  // CRAP}

  float fSpeedLen;
  interface IBasePathUnit *pUnit;

public:
  CPresizePath() : eState(EPPS_WAIT_FOR_INIT), vEndPoint(VNULL2), vEndDir(VNULL2), wDesiredDir(0), fSpeedLen(0.0f), pUnit(nullptr) {}
  CPresizePath(interface IBasePathUnit *pUnit, const class CVec2 &vEndPoint, const class CVec2 &vEndDir);
  const CVec2 &GetFinishPoint() const override { return vEndPoint; }

  bool IsFinished() const override;
  const CVec3 GetPoint(NTimer::STime timeDiff) override;

  bool Init(interface IBasePathUnit *pUnit, IPath *pPath, bool bSmoothTurn = true, bool bCheckTurn = true) override;
  bool InitByFormationPath(class CFormation *pFormation, interface IBasePathUnit *pUnit) override;
  bool Init(interface IMemento *pMemento, interface IBasePathUnit *pUnit) override;
  void Stop() override;
  float &GetSpeedLen() override;
  void NotifyAboutClosestThreat(interface IBasePathUnit *pCollUnit, float fDist) override;
  void SlowDown() override;
  bool CanGoBackward() const override;
  bool CanGoForward() const override;
  void GetNextTiles(std::list<SVector> *pTiles) override;
  CVec2 GetShift(int nToShift) const override;
  IMemento *GetMemento() const override;

  float GetCurvatureRadius() const override { return 0.0f; }

  bool IsWithFormation() const override { return false; }
  void SetOwner(interface IBasePathUnit *_pUnit) override;
  IBasePathUnit *GetOwner() const override { return pUnit; }
};

#endif // __PRESIZE_PATH_H__