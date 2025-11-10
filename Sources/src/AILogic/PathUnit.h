#ifndef __PATH_UNIT_H__
#define __PATH_UNIT_H__

#pragma once

#include "../Common/Actions.h"

interface IStaticPathFinder;
interface ISmoothPath;
interface ICollision;
interface IMemento;

class CPathUnit : public IRefCount
{
  DECLARE_SERIALIZE;

  CPtr<IStaticPathFinder> pPathFinder;

  CObj<ICollision> pCurCollision;

  CPtr<IMemento> pPathMemento;
  CObj<ICollision> pCollMemento;
  CPtr<CPathUnit> pLastPushByHardCollUnit;

  // "right direction", i.e. 
  SAINotifyPlacement placement;
  SVector curTile;
  // vector along the direction of movement
  CVec2 dirVec;
  // the right direction or moving backwards
  bool bRightDir;

  bool bLocking;
  // is it currently turning?
  bool bTurning;
  // if turning, then in what direction?
  WORD desDir;
  bool bFoolStop;
  bool bFixUnlock;
  WORD wDirAtBeginning;

  bool bOnLockedTiles;
  NTimer::STime checkOnLockedTime;

  int nCollisions;
  NTimer::STime collStayTime;

  CVec2 vSuspendedPoint;
  bool bTurnCalled;
  SVector lastKnownGoodTile;

  bool bIdle;
  NTimer::STime nextSecondPathSegmTime;

  //
  const CVec2 GetCenterShift() const;
  // choose how to turn - to face the back towards the movement or the front
  void ChooseDirToTurn(const WORD &newDir);
  // turn when you already know which way
  bool MakeTurnToDir(WORD newDir);

  bool CanMake180DegreesTurn(SRect rect);

protected:
  class CAIUnit *pOwner;
  CVec2 speed;
  NTimer::STime stayTime;

  void CalculateIdle();

public:
  CPathUnit() : pOwner(nullptr) {}
  virtual void Init(class CAIUnit *pOwner, const CVec2 &center, int z, WORD dir, WORD id);

  // sets the direction of movement. 
  virtual void SetRightDir(bool bRightDir);
  virtual bool GetRightDir() const { return bRightDir; }
  //
  virtual void GetPlacement(struct SAINotifyPlacement *pPlacement, NTimer::STime timeDiff) const;
  virtual void GetSpeed3(CVec3 *pSpeed) const;
  virtual const float GetZ() const { return placement.z; }

  virtual void FirstSegment();
  virtual void SecondSegment(bool bUpdate = true);
  // returns whether I went or not
  virtual bool SendAlongPath(interface IStaticPath *pStaticPath, const CVec2 &vShift, bool bSmoothTurn = true);
  virtual bool SendAlongPath(interface IPath *pPath);

  bool CanGoByDir(const CVec2 &dir, const SRect &forceRect, SRect bannedRect, CVec2 forceSpeed, int *numIntersect, float *distance, int *nBadness);
  interface IPath *CreatePathByDirection(const CVec2 &startPoint, const CVec2 &dir, const CVec2 &finishPoint);

  bool IsStopped() const;

  virtual const CVec2 &GetCenter() const { return placement.center; }
  virtual const float GetRotateSpeed() const;
  const SVector GetTile() const { return curTile; }
  virtual const CVec2 &GetSpeed() const { return speed; }
  virtual const float GetSpeedLen() const;
  virtual const float GetMaxPossibleSpeed() const;
  virtual const int GetBoundTileRadius() const;
  virtual interface ISmoothPath *GetCurPath() const { return GetSmoothPath(); }

  interface ICollision *GetCollision() const { return pCurCollision; }
  void SetCollision(ICollision *pCollision);

  // directions of movement are given
  virtual const CVec2 &GetDirVector() const;
  virtual const WORD GetDir() const;
  // front direction
  virtual const WORD GetFrontDir() const { return placement.dir; }

  // the direction the front is facing
  const CVec2 GetFrontDirVec() const
  {
    if (bRightDir) return dirVec;
    return -dirVec;
  }

  // bCanBackward - can turn either backwards or forwards, at your choice
  // bForward - turn forward
  virtual bool TurnToDir(const WORD &newDir, bool bCanBackward = true, bool bForward = true);

  virtual interface IStaticPathFinder *GetPathFinder() const { return pPathFinder; }

  virtual void StopUnit();
  virtual void StopTurning();
  virtual void ForceGoByRightDir() { bRightDir = true; }
  void UnsetDirUpdate() { wDirAtBeginning = GetDir(); }
  const WORD GetDirAtTheBeginning() const { return wDirAtBeginning; }
  virtual bool IsIdle() const;
  bool IsTurning() const { return bTurning; }
  bool IsLockingTiles() const { return bLocking; }

  virtual void LockTiles(bool bUpdate = true);
  void ForceLockingTiles(bool bUpdate = true);

  // localizes in any case, if this is possible in principle
  void LockTilesForEditor();
  // bUpdate - whether to update maxes when unlocking
  virtual void UnlockTiles(bool bUpdate);
  void FixUnlocking() { bFixUnlock = true; }
  void UnfixUnlocking() { bFixUnlock = false; }

  virtual const CVec2 GetAABBHalfSize() const;

  const SRect GetUnitRect() const;
  virtual const SRect GetUnitRectForLock() const;
  const SRect GetFullSpeedRect(bool bForInfantry) const;
  const SRect GetSpeedRect(bool bForInfantry) const;
  const SRect GetSmallRect() const;

  //
  bool CanShootUnit(const CPathUnit *pTarget) const;
  bool TooClose(const CPathUnit *pTarget) const;

  virtual void SetNewCoordinates(const CVec3 &newCenter, bool bStopUnit = true);
  virtual void SetNewCoordinatesForEditor(const CVec3 &newCenter) { SetNewCoordinates(newCenter); }
  virtual void SetCoordWOUpdate(const CVec3 &newCenter);
  virtual void SetFrontDirWOUpdate(WORD newDir);
  bool CanSetNewCoord(const CVec3 &newCenter);

  // update the direction according to the new direction of movement
  void UpdateDirection(WORD newDir);
  void UpdateDirection(const CVec2 &dirVec);
  virtual void UpdateDirectionForEditor(const CVec2 &dirVec);
  bool CanSetNewDir(const CVec2 &newDir);

  //
  bool CanLockTiles(bool bForceLocking = false) const;
  bool CanUnlockTiles() const;

  virtual bool CanTurnToFrontDir(WORD wDir);

  //
  const SUnitBaseRPGStats *GetStats() const;
  class CAIUnit *GetOwner() const { return pOwner; }
  virtual BYTE GetAIClass() const;
  int GetID() const;

  void IncNCollisions() { ++nCollisions; }
  const int GetNCollisions() const { return nCollisions; }
  void NullCollisions() { nCollisions = 0; }

  void SetLastPushByHardCollUnit(class CPathUnit *pUnit) { pLastPushByHardCollUnit = pUnit; }
  CPathUnit *GetLastPushByHardCollUnit() const { return pLastPushByHardCollUnit; }
  const NTimer::STime GetStayTime() const { return stayTime; }

  void NullCollStayTime() { collStayTime = 0; }
  void UpdateCollStayTime(NTimer::STime candStayTime);
  const NTimer::STime GetCollStayTime() const { return collStayTime; }

  // Is it possible to rotate to the vNewDir direction if smallRect is taken as the bounding box
  bool CanRotateTo(SRect smallRect, const CVec2 &vNewDir, bool bWithUnits, bool bCanGoBackward = true);
  void SetSuspendedPoint(const CVec2 &vPoint) { vSuspendedPoint = vPoint; }
  bool HasSuspendedPoint() const { return vSuspendedPoint.x != -1.0f; }
  // checks if wNewDir can be rotated,
  // if not, returns false and sends along the path to a point where a full turn can be made, if such a point is found
  virtual bool CheckToTurn(WORD wNewDir);

  // destruction of objects that were run over
  void CheckForDestroyedObjects(const CVec2 &center) const;
  virtual IStaticPath *CreateBigStaticPath(const CVec2 &vStartPoint, const CVec2 &vFinishPoint, interface IPointChecking *pPointChecking);

  virtual ISmoothPath *GetSmoothPath() const = 0;
  virtual void SetCurPath(interface ISmoothPath *pNewPath) = 0;
  virtual void RestoreDefaultPath() = 0;

  virtual bool IsInOneTrain(interface IBasePathUnit *pUnit) const;
  virtual bool CanMove() const { return true; }
  virtual bool CanRotate() const;

  virtual const SVector GetLastKnownGoodTile() const;

  // for restore default aviation path
  virtual void InitAviationPath(const SMechUnitRPGStats *pStats) { NI_ASSERT_T(false, "wrong call"); }

  virtual void TrackDamagedState(const bool bTrackDamaged) {}

  const NTimer::STime GetNextSecondPathSegmTime() const { return nextSecondPathSegmTime; }
};

class CSimplePathUnit : public CPathUnit
{
  OBJECT_COMPLETE_METHODS(CSimplePathUnit);
  DECLARE_SERIALIZE;

  CPtr<ISmoothPath> pSmoothPath;
  CPtr<ISmoothPath> pDefaultPath;// for temporary storage of default smooth path
public:
  CSimplePathUnit() {}

  void Init(class CAIUnit *pOwner, const CVec2 &center, int z, WORD dir, WORD id) override;

  void InitAviationPath(const SMechUnitRPGStats *pStats) override;

  ISmoothPath *GetSmoothPath() const override;
  void SetCurPath(interface ISmoothPath *pNewPath) override;
  void RestoreDefaultPath() override;
};

#endif // __PATH_UNIT_H__