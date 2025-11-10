#ifndef __STANDART_PATH_H__
#define __STANDART_PATH_H__
// ///////////////////////////////////////////////////////////// 
#pragma once
// ///////////////////////////////////////////////////////////// 
#include "Path.h"
// ///////////////////////////////////////////////////////////// 
// **********************************************************************
// *CCommonStaticPath*
// **********************************************************************
// ///////////////////////////////////////////////////////////// 
// a large static path along which units walk
class CCommonStaticPath : public IStaticPath
{
  OBJECT_COMPLETE_METHODS(CCommonStaticPath);
  DECLARE_SERIALIZE;

  std::vector<SVector> path;
  int nLen;
  // in tile coordinates
  SVector startTile, finishTile;
  CVec2 finishPoint;

  void SetPath(const interface IStaticPathFinder &staticPathFinder, int nLen, const CVec2 &finishPoint);

public:
  CCommonStaticPath() : path(0), nLen(-1) {}
  CCommonStaticPath(const interface IStaticPathFinder &staticPathFinder, const CVec2 &finishPoint);
  CCommonStaticPath(const interface IStaticPathFinder &staticPathFinder, int nLen, const CVec2 &finishPoint);

  // move the starting tile to path[nStart]
  void MoveStartTileTo(int nStart);
  // make the final tile on path[nFinish - 1]
  void MoveFinishTileTo(int nFinish);
  // move endpoint to vMove
  void MoveFinishPointBy(const CVec2 &vMove) override;

  const int GetLength() const override { return nLen; }

  virtual const SVector GetTile(const int n) const
  {
    NI_ASSERT_T(n >= 0 && n < nLen, "Wrong point number");
    return path[n];
  }

  const SVector GetStartTile() const override { return startTile; }
  const SVector GetFinishTile() const override { return finishTile; }
  const CVec2 &GetFinishPoint() const override { return finishPoint; }
};

// ///////////////////////////////////////////////////////////// 
// **********************************************************************
// * Essential for units and conventional ground vehicles *
// **********************************************************************
// ///////////////////////////////////////////////////////////// 
// unit path (walk along a large static path)
class CStandartPath : public IPath
{
  OBJECT_COMPLETE_METHODS(CStandartPath);
  DECLARE_SERIALIZE;

  CPtr<IStaticPathFinder> pPathFinder;
  CPtr<CCommonStaticPath> pStPath;

  int nBoundTileRadius;
  BYTE aiClass;

  // start and end points
  CVec2 startPoint, finishPoint;
  // current tile on large static path ( pStPath )
  SVector curStPathTile;
  // shift from the end point (eg, due to walking in a group)
  CVec2 vShift;
  // the number of the current tile on a small static. 
  int nCurTile;

  // point number on a large static. 
  int nCurStaticPoint, nCurPathPoint;
  // the small path is too long,
  bool bSmallPathTooLong;

  enum { ENPathPoints = 2 * SAIConsts::MAX_LENGTH_OF_SMALL_PATH + 1 };

  // small static points 
  SVector pathPoints[ENPathPoints + 2 * SAIConsts::MAX_LENGTH_OF_SMALL_PATH];
  SVector lastKnownGoodTile;

  std::vector<SVector> insertedTiles;
  int nInsertedTiles;
  int nCurInsertedTile;

  //
  // count small static. 
  bool CalculateNewPath(bool bShift);
  void CalculateSmallPath(bool bLastStep);
  void SaveSmallPath(int nToSave);
  void MoveDistantAim(int nMove);
  void InitByStaticPath(interface IStaticPath *pStPath, const CVec2 &startPoint, const CVec2 &finishPoint);

  //
  static const BYTE GetNextPos(BYTE n);

public:
  CStandartPath() : pPathFinder(nullptr), pStPath(nullptr) {}
  CStandartPath(int nBoundTileRadius, BYTE aiClass, interface IStaticPathFinder *pPathFinder, interface IStaticPath *pStaticPath, const CVec2 &startPoint, const CVec2 &finishPoint, const SVector &lastKnownGoodTile);

  const CVec2 &GetStartPoint() const override { return startPoint; }
  const CVec2 &GetFinishPoint() const override { return finishPoint; }
  bool IsFinished() const override { return nCurTile == nCurPathPoint; }

  const CVec2 PeekPoint(int nShift) override;
  void Shift(int nShift) override;

  void RecoverState(const CVec2 &point, const SVector &lastKnownGoodTile) override;
  void Recalculate(const CVec2 &point, const SVector &lastKnownGoodTile) override;

  void InsertTiles(const std::list<SVector> &tiles) override;

  bool CanGoBackward(interface IBasePathUnit *pUnit) override;
  bool ShouldCheckTurn() const override { return true; }
};

// ///////////////////////////////////////////////////////////// 
// **********************************************************************
// *go in the direction*
// **********************************************************************
// ///////////////////////////////////////////////////////////// 
class CStandartDirPath : public IPath
{
  OBJECT_COMPLETE_METHODS(CStandartDirPath);
  DECLARE_SERIALIZE;

  CVec2 dir;
  CVec2 startPoint, finishPoint;
  CVec2 curPoint;
  bool bFinished;

public:
  CStandartDirPath() : bFinished(true) {}
  CStandartDirPath(const CVec2 &startPoint, const CVec2 &dir, const CVec2 &finishPoint);

  bool IsFinished() const override;

  const CVec2 &GetStartPoint() const override { return startPoint; }
  const CVec2 &GetFinishPoint() const override;

  const CVec2 PeekPoint(int nShift) override;
  void Shift(int nShift) override;

  void RecoverState(const CVec2 &point, const SVector &lastKnownGoodTile) override;
  void Recalculate(const CVec2 &point, const SVector &lastKnownGoodTile) override;

  bool CanGoBackward(interface IBasePathUnit *pUnit) override;
  bool ShouldCheckTurn() const override { return false; }
};

// ///////////////////////////////////////////////////////////// 
// **********************************************************************
// *CStandartSmoothPathMemento*
// **********************************************************************
// ///////////////////////////////////////////////////////////// 
class CStandartSmoothPathMemento : public IMemento
{
  OBJECT_COMPLETE_METHODS(CStandartSmoothPathMemento);
  DECLARE_SERIALIZE;

  CPtr<IPath> pPath;
  CPtr<CFormation> pFormation;

public:
  friend class CStandartSmoothMechPath;
  friend class CStandartSmoothSoldierPath;
};

// ///////////////////////////////////////////////////////////// 
#endif // __STANDART_PATH_H__