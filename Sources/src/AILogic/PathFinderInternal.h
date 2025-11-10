#ifndef __PATH_FINDER_INTERNAL_H__
#define __PATH_FINDER_INTERNAL_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "PathFinder.h"
// //////////////////////////////////////////////////////////// 
// **********************************************************************
// * Essential for units and conventional ground vehicles *
// **********************************************************************
// //////////////////////////////////////////////////////////// 
class CStandartPathFinder : public IStaticPathFinder
{
  OBJECT_NORMAL_METHODS(CStandartPathFinder);
  DECLARE_SERIALIZE;

  interface IPointChecking *pChecking;

  int nBoundTileRadius;
  int upperLimit;
  BYTE aiClass;
  bool longPath;
  SVector startPoint, finishPoint;
  SVector lastKnownGoodTile;

  int nLength, nStart;

  int minDistance, minPointNum;
  // found the point
  bool bFinished;

  enum { COEFFICIENT = 7 };

  enum { MAX_STOP_POINTS = SAIConsts::MAX_MAP_SIZE * COEFFICIENT };

  enum
  {
    STEP_LENGTH_THERE = 30, MAX_NUM_OF_ATTEMPTS_THERE = 4,
    STEP_LENGTH_BACK = 10, MAX_NUM_OF_ATTEMPTS_BACK = 12,
    STEP_LENGTH_THERE_SHORT = 5, MAX_NUM_OF_ATTEMPTS_THERE_SHORT = 4,
    STEP_LENGTH_BACK_SHORT = 2, MAX_NUM_OF_ATTEMPTS_BACK_SHORT = 12
  };

  enum { TOLERANCE = 64, TOLERANCE_SHORT = 16 };

  static SVector stopPoints[SAIConsts::INFINITY_PATH_LIMIT + 1];
  static SVector addPoints[SAIConsts::INFINITY_PATH_LIMIT + 1];
  static BYTE mapBuf[SAIConsts::MAX_MAP_SIZE][SAIConsts::MAX_MAP_SIZE];
  int nCyclePoints;
  static int cyclePoints[SAIConsts::INFINITY_PATH_LIMIT + 1];
  static int segmBegin[SAIConsts::INFINITY_PATH_LIMIT + 1];

  //
  void LineSmoothing(int STEP_LENGTH_THERE, int MAX_NUM_OF_ATTEMPTS_THERE,
                     int STEP_LENGTH_BACK, int MAX_NUM_OF_ATTEMPTS_BACK);

  const SVector CalculateHandPath(const SVector &blockPoint, const SVector &dir, const SVector &finish);
  const SVector CalculateSimplePath(const SVector &blockPoint, const SVector &dir, const SVector &finish);
  bool CanGoTowardPoint(const SVector &start, const SVector &finish);

  bool Walkable(const SVector &start, const SVector &finish);
  const int SavePathThere(const SVector &start, const SVector &finish, SVector *buf, int nLen);
  const int SavePathBack(const SVector &start, const SVector &finish, SVector *buf, int nLen);

  bool CheckFakePath(SVector point);
  const SVector LookForFakePathBegin();

  void EraseCycles();
  void AnalyzePoint(const SVector &point, int num);
  // if the unit is now on an impassable tile, then you can length the path from pointFrom then lastKnownGoodTile, and then look for the path
  const int GetAdditionalPathLength(const SVector &pointFrom);

public:
  CStandartPathFinder() : pChecking(nullptr), nBoundTileRadius(0), upperLimit(0), aiClass(AI_CLASS_HUMAN), nLength(-1)
  {
    memset(*mapBuf, 0, sizeof(mapBuf));
    cyclePoints[0] = 0;
  }

  void SetPathParameters(int nBoundTileRadius, BYTE aiClass, interface IPointChecking *pChecking, const CVec2 &startPoint, const CVec2 &finishPoint, int upperLimit, bool longPath, const SVector &lastKnownGoodTile) override;

  // finding a path without any improvements
  bool CalculatePath() override;
  // finding a path to a tile without loops
  void CalculatePathWOCycles() override;

  void SmoothPath() override
  {
    if (longPath) LineSmoothing(STEP_LENGTH_THERE, MAX_NUM_OF_ATTEMPTS_THERE, STEP_LENGTH_BACK, MAX_NUM_OF_ATTEMPTS_BACK);
    else LineSmoothing(STEP_LENGTH_THERE_SHORT, MAX_NUM_OF_ATTEMPTS_THERE_SHORT, STEP_LENGTH_BACK_SHORT, MAX_NUM_OF_ATTEMPTS_BACK_SHORT);
  }

  const int GetPathLength() const override { return nLength; }

  const SVector GetStopTile(int n) const override
  {
    NI_ASSERT_T(n >= 0 && n < nLength, "Wrong number of stop point");
    return addPoints[n];
  }

  const void GetStopTiles(void *buf, int len) const override
  {
    NI_ASSERT_T(nLength > 0 && len > 0 && len <= nLength, "Wrong number of stop points");
    memcpy(buf, addPoints, len * sizeof(SVector));
  }

  const SVector GetStartTile() const override { return startPoint; }
  const SVector GetFinishTile() const override { return finishPoint; }

  IPath *CreatePathByDirection(const CVec2 &startPoint, const CVec2 &dir, const CVec2 &finishPoint, int nBoundTileRadius) override;
};

// //////////////////////////////////////////////////////////// 
// **********************************************************************
// * the essence of airplanes *
// **********************************************************************
// //////////////////////////////////////////////////////////// 
class CPlanePathFinder : public IStaticPathFinder
{
  OBJECT_NORMAL_METHODS(CPlanePathFinder);
  DECLARE_SERIALIZE;

  interface IPointChecking *pChecking;

  SVector startTile, finishTile;

public:
  void SetPathParameters(int nBoundTileRadius, BYTE aiClass, interface IPointChecking *pChecking, const CVec2 &startPoint, const CVec2 &finishPoint, int upperLimit, bool longPath, const SVector &lastKnownGoodTile) override;

  // finding a path without any improvements
  bool CalculatePath() override { return true; }
  // finding a path to a point without loops
  void CalculatePathWOCycles() override {}
  void SmoothPath() override {}

  const int GetPathLength() const override { return 0; }
  const SVector GetStopTile(int n) const override { return startTile; }
  const void GetStopTiles(void *buf, int len) const override {}

  const SVector GetStartTile() const override { return startTile; }
  const SVector GetFinishTile() const override { return finishTile; }

  interface IPath *CreatePathByDirection(const CVec2 &startPoint, const CVec2 &dir, const CVec2 &finishPoint, int nBoundTileRadius) override;
};

// //////////////////////////////////////////////////////////// 
#endif // __PATH_FINDER_INTERNAL_H__