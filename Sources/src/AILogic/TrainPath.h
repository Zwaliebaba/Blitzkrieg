#ifndef __TRAIN_PATH_H__
#define __TRAIN_PATH_H__

#pragma once

#include "Path.h"

class CEdgePoint;
interface IEdge;

struct SPathEdge
{
  DECLARE_SERIALIZE;

public:
  CPtr<CEdgePoint> pFirstPoint;
  CPtr<CEdgePoint> pLastPoint;

  SPathEdge() {}
  SPathEdge(CEdgePoint *_pFirstPoint, CEdgePoint *_pLastPoint) : pFirstPoint(_pFirstPoint), pLastPoint(_pLastPoint) {}
};

class CTrainPath : public IStaticPath
{
  OBJECT_COMPLETE_METHODS(CTrainPath);
  DECLARE_SERIALIZE;

  std::list<SPathEdge> edges;

  CVec2 vStartPoint;
  CVec2 vFinishPoint;

  // 
public:
  CTrainPath() {}
  CTrainPath(interface IStaticPathFinder *pPathFinder, class CTrainPathUnit *pTrain);

  const SVector GetStartTile() const override;
  const SVector GetFinishTile() const override;
  const CVec2 &GetFinishPoint() const override;

  const int GetLength() const override { return edges.size(); }
  void MoveFinishPointBy(const CVec2 &vMove) override {}

  std::list<SPathEdge>::iterator GetStartEdgeIter() { return edges.begin(); }
  std::list<SPathEdge>::iterator GetEndEdgesIter() { return edges.end(); }
  CEdgePoint *GetFirstPoint(std::list<SPathEdge>::iterator iter);
  CEdgePoint *GetLastPoint(std::list<SPathEdge>::iterator iter);

  const CVec2 GetStartPoint() const { return vStartPoint; }
  // starting direction
  const CVec2 GetDirToGo();
};

class CTrainSmoothPath : public ISmoothPath
{
  OBJECT_COMPLETE_METHODS(CTrainSmoothPath);
  DECLARE_SERIALIZE;

  CTrainPathUnit *pOwner;
  CPtr<CTrainPath> pTrainPath;
  NTimer::STime lastUpdateTime;
  float fSpeed;
  bool bFinished;

  struct SPathPoint
  {
    DECLARE_SERIALIZE;

  public:
    std::list<SPathEdge>::iterator iter;
    CPtr<CEdgePoint> pPoint;
  };

  struct SCarriagePos
  {
    DECLARE_SERIALIZE;

  public:
    SPathPoint frontWheel;
    SPathPoint backWheel;
  };

  std::vector<SCarriagePos> carriages;
  int nRecalculating;

  bool bRecalculatedPath;
  CVec2 vRealFinishPoint;

  // for saver/load
  int iteratorShift;
  bool bJustLoaded;

  //
  void InitTrain();
  // move the front wheel of the car n by a distance fDist
  void MoveFrontWheel(int n, float fDist);

  void CheckPath();
  void DelSharpAngles();
  void LoadIterators();
  void FinishPath();

public:
  CTrainSmoothPath() : lastUpdateTime(0), fSpeed(0), bFinished(true), nRecalculating(0), bRecalculatedPath(false), bJustLoaded(false) {}

  bool Init(IStaticPath *pTrainPath);

  bool Init(interface IBasePathUnit *pUnit, IPath *pPath, bool bSmoothTurn, bool bCheckTurn = true) override;
  bool Init(interface IMemento *pMemento, interface IBasePathUnit *pUnit) override;
  bool InitByFormationPath(class CFormation *pFormation, interface IBasePathUnit *pUnit) override;

  const CVec2 &GetFinishPoint() const override { return pTrainPath->GetFinishPoint(); }

  bool IsFinished() const override;

  void Stop() override {}

  const CVec3 GetPoint(NTimer::STime timeDiff) override;
  float &GetSpeedLen() override;

  void NotifyAboutClosestThreat(interface IBasePathUnit *pCollUnit, float fDist) override;
  void SlowDown() override {}

  bool CanGoBackward() const override { return false; }
  bool CanGoForward() const override { return true; }
  void GetNextTiles(std::list<SVector> *pTiles) override { pTiles->clear(); }
  // error up to SAIConsts::SPLINE_STEP, used mainly for formation
  CVec2 GetShift(const int nToShift) const override { return VNULL2; }

  IMemento *GetMemento() const override;
  // radius of curvance. 
  float GetCurvatureRadius() const override { return 0.0f; }
  CVec2 GetCurvatureCenter() const override { return CVec2(1e15, 1e15); }

  bool IsWithFormation() const override { return false; }

  void GetSpeed3(CVec3 *vSpeed) const override
  {
    // CRAP{ until Vitalik in his ways begins to calculate the speed
    *vSpeed = VNULL3;
    // CRAP}
  }

  CEdgePoint *GetBackWheelPoint(int n) const;
  CEdgePoint *GetFrontWheelPoint(int n) const;

  void SetNewFrontWheel(int n, CEdgePoint *pNewPoint);
  void SetNewBackWheel(int n, CEdgePoint *pNewPoint);

  // for save/load
  void SetOwner(interface IBasePathUnit *pUnit) override;
  IBasePathUnit *GetOwner() const override;
};

class CTrainSmoothPathMemento : public IMemento
{
  OBJECT_COMPLETE_METHODS(CTrainSmoothPathMemento);
  DECLARE_SERIALIZE;

public:
  CPtr<CTrainPath> pPath;

  CTrainSmoothPathMemento() {}
  CTrainSmoothPathMemento(CTrainPath *pPath);
};

#endif // __TRAIN_PATH_H__