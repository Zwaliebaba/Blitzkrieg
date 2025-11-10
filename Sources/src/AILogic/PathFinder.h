#ifndef __PATH_FINDER_H__
#define __PATH_FINDER_H__

#pragma once

interface IPointChecking : IRefCount
{
  virtual bool IsGoodTile(const SVector &curTile) const = 0;
};

interface IStaticPathFinder : IRefCount
{
  virtual void SetPathParameters(int nBoundTileRadius, BYTE aiClass, interface IPointChecking *pChecking, const CVec2 &startPoint, const CVec2 &finishPoint, int upperLimit, bool longPath, const SVector &lastKnownGoodTile) = 0;

  // finding a path without any improvements
  virtual bool CalculatePath() = 0;
  // finding a path to a point without loops
  virtual void CalculatePathWOCycles() = 0;
  virtual void SmoothPath() = 0;

  virtual const int GetPathLength() const = 0;
  virtual const SVector GetStopTile(int n) const = 0;
  virtual const void GetStopTiles(void *buf, int len) const = 0;

  virtual const SVector GetStartTile() const = 0;
  virtual const SVector GetFinishTile() const = 0;

  virtual interface IPath *CreatePathByDirection(const CVec2 &startPoint, const CVec2 &dir, const CVec2 &finishPoint, int nBoundTileRadius) = 0;
};

interface IStaticPath *CreateStaticPathToPoint(const CVec2 &finishPoint, const CVec2 &vShift, interface IBasePathUnit *pUnit, bool bCanGoOutOfRadius = false);
interface IStaticPath *CreateStaticPathToPoint(const CVec2 &startPoint, const CVec2 &finishPoint, const CVec2 &vShift, interface IBasePathUnit *pUnit, bool bCanGoOutOfRadius = false);
interface IStaticPath *CreateStaticPathForAttack(interface IBasePathUnit *pUnit, class CAIUnit *pTarget, float fRangeMin, float fRangeMax, float fRandomCant = 0.0f);
interface IStaticPath *CreateStaticPathForStObjAttack(interface IBasePathUnit *pUnit, class CStaticObject *pObj, float fRangeMin, float fRangeMax);
interface IStaticPath *CreateStaticPathForSideAttack(interface IBasePathUnit *pUnit, class CAIUnit *pTarget, const CVec2 &attackDir, float fRangeMin, float fRangeMax, float fDistToPoint, WORD wHalfAngle = 8192);
interface IStaticPath *CreatePathWithChecking(interface IBasePathUnit *pUnit, const SVector &vTargetTile, IPointChecking *pPointChecking);
bool CanUnitApproachToUnitByPath(const class CAIUnit *Moving, const interface IStaticPath *Path, const class CAIUnit *Standing);
bool CanUnitApproachToPointByPath(const class CAIUnit *Moving, const IStaticPath *Path, const class CVec2 &point);
bool CanUnitApproachToObjectByPath(const class CAIUnit *Moving, const IStaticPath *Path, const class CStaticObject *point);
bool IsUnitNearObject(const class CAIUnit *pUnit, const class CStaticObject *pObj);
bool IsUnitNearUnit(const class CAIUnit *pUnit1, const class CAIUnit *pUnit2);
// bool IsUnitNearPoint( const class CAIUnit * pUnit1, const class CVec2 & point, const float factor=1.2f );
bool IsUnitNearPoint(const class CAIUnit *pUnit1, const class CVec2 &point, int add = 0);
bool IsPointNearPoint(const class CVec2 &point1, const class CVec2 &point2);

#endif // __PATH_FINDER_H__