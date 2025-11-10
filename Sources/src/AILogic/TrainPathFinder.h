#ifndef __TRAIN_PATH_FINDER_H__
#define __TRAIN_PATH_FINDER_H__
// //////////////////////////////////////////////////////////// 
#pragma once
// //////////////////////////////////////////////////////////// 
#include "PathFinder.h"
#include "RailRoadGraph.h"
// //////////////////////////////////////////////////////////// 
class CEdgePoint;
class CTrainPathUnit;

// //////////////////////////////////////////////////////////// 
class CTrainPathFinder : public IStaticPathFinder
{
  OBJECT_COMPLETE_METHODS(CTrainPathFinder);
  DECLARE_SERIALIZE;

  CPtr<CTrainPathUnit> pTrain;
  CPtr<CEdgePoint> pStartEdgePoint;
  CPtr<CEdgePoint> pFinishEdgePoint;
  CVec2 finishPoint;

  std::list<int> bestPath;
  float fBestPathLen;
  std::list<int>::iterator iter;
  //
  void AnalyzePath(int v1, int v2, float fDistToV1, CEdgePoint *pPoint);

public:
  virtual void SetPathParameters(class CTrainPathUnit *pTrain, const CVec2 &finishPoint);

  void SetPathParameters(const int nBoundTileRadius, const BYTE aiClass, interface IPointChecking *pChecking, const CVec2 &startPoint, const CVec2 &finishPoint, const int upperLimit, const bool longPath, const SVector &lastKnownGoodTile) override {}

  // finding a path without any improvements
  bool CalculatePath() override;
  // finding a path to a point without loops
  void CalculatePathWOCycles() override {}
  void SmoothPath() override {}

  const int GetPathLength() const override;

  const SVector GetStopTile(int n) const override
  {
    NI_ASSERT_T(false, "Not defined for train path");
    return SVector(0, 0);
  }

  const void GetStopTiles(void *buf, int len) const override { NI_ASSERT_T(false, "Not defined for train path"); }

  const SVector GetStartTile() const override;
  const SVector GetFinishTile() const override;

  CEdgePoint *GetStartEdgePoint() const;
  CEdgePoint *GetFinishEdgePoint() const;

  // won't save
  void StartPathIterating();
  const int GetCurPathNode() const;
  void Iterate();

  interface IPath *CreatePathByDirection(const CVec2 &startPoint, const CVec2 &dir, const CVec2 &finishPoint, const int nBoundTileRadius) override { return nullptr; }
};

// //////////////////////////////////////////////////////////// 
#endif // __TRAIN_PATH_FINDER_H__