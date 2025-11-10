#ifndef __TRAIN_PATH_UNIT_H__
#define __TRAIN_PATH_UNIT_H__

#pragma once

#include "PathUnit.h"
#include "BasePathUnit.h"

class CTrainSmoothPath;
class CEdgePoint;
class CCarriagePathUnit;
interface IEdge;
interface IStaticPath;

class CTrainPathUnit : public IRefCount, public IBasePathUnit
{
  OBJECT_COMPLETE_METHODS(CTrainPathUnit);
  DECLARE_SERIALIZE;

  std::vector<CPtr<CCarriagePathUnit>> carriages;
  std::vector<std::list<int>> nodesInside;
  std::vector<std::list<int>> intermNodes;

  CPtr<CTrainSmoothPath> pSmoothPath;
  CPtr<CEdgePoint> pCurEdgePoint;

  CVec2 vCenter;
  CVec2 vSpeed;
  CVec2 vDir;
  float fMaxPossibleSpeed;
  float fPassability;

  float fTrainLength;
  bool bFrontDir;
  bool bCanMove;

  std::unordered_set<int> damagedTrackCarriages;

  CPtr<IStaticPath> pPathToMove;

public:
  CTrainPathUnit() : fTrainLength(0), bFrontDir(true), bCanMove(false) {}
  CTrainPathUnit(class CAIUnit *pOwner) : fTrainLength(0), bFrontDir(true), bCanMove(false) {}
  virtual void Init(const CVec2 &center, const int z, const WORD dir, const WORD id) {}
  // initialize with locomotive pUnit
  bool InitBy(class CCarriagePathUnit *pUnit);

  virtual ISmoothPath *GetSmoothPath() const;
  const float GetRotateSpeed() const override { return 0; }

  interface ISmoothPath *GetCurPath() const override;
  virtual void SetCurPath(interface ISmoothPath *pNewPath) { NI_ASSERT_T(false, "Train can't change smooth path"); }
  virtual void RestoreDefaultPath() { NI_ASSERT_T(false, "Train can't change smooth path"); }

  virtual void SecondSegment(bool bUpdate = true);
  IStaticPath *CreateBigStaticPath(const CVec2 &vStartPoint, const CVec2 &vFinishPoint, interface IPointChecking *pPointChecking) override;
  // returns whether I went or not
  bool SendAlongPath(interface IStaticPath *pStaticPath, const CVec2 &vShift, bool bSmoothTurn = true) override;
  bool SendAlongPath(interface IPath *pPath) override;

  CEdgePoint *GetCurEdgePoint();
  void SetCurEdgePoint(CEdgePoint *pEdgePoint);

  void AddCarriage(class CCarriagePathUnit *pCarriage);
  const float GetTrainLength() const { return fTrainLength; }

  const int GetNCarriages() const { return carriages.size(); }
  CCarriagePathUnit *GetCarriage(int n);
  // distance from the rear wheel of car n to the front wheel of car m, if m is connected to m
  const float GetDistFromBackToFrontWheel(int n, int m);
  const float GetDistFromFrontToBackWheel(int n, int m);
  void PushNodesToFrontCarriage(std::list<int> &newNodes);

  void SetBackWheel(int n);
  void SetFrontWheel(int n);

  void ChangeDirection(bool bNewFrontDir);
  bool IsFrontDir() const { return bFrontDir; }

  void GetTrainNodes(std::list<int> *pNodesOfTrain);

  // IBasePathUnit
  void SetRightDir(bool _bRightDir) override {}
  bool GetRightDir() const override { return true; }

  const WORD GetID() const override;
  const CVec2 &GetCenter() const override;
  const float GetZ() const override;
  const SVector GetTile() const override { return AICellsTiles::GetTile(GetCenter()); }
  const float GetMaxSpeedHere(const CVec2 &point, bool bAdjust = true) const override;
  const float GetMaxPossibleSpeed() const override;
  const float GetPassability() const override;
  bool CanMove() const override;
  bool CanMovePathfinding() const override;
  const CVec2 &GetSpeed() const override;
  const int GetBoundTileRadius() const override;
  const WORD GetDir() const override;
  const WORD GetFrontDir() const override;
  const CVec2 &GetDirVector() const override;
  const CVec2 GetAABBHalfSize() const override;
  void SetCoordWOUpdate(const CVec3 &newCenter) override;
  void SetNewCoordinates(const CVec3 &newCenter, bool bStopUnit = true) override;

  const SRect GetUnitRectForLock() const override;

  bool TurnToDir(const WORD &newDir, const bool bCanBackward = true, const bool bForward = true) override { return false; }
  bool TurnToUnit(const CVec2 &targCenter) override { return false; }
  void TurnAgainstUnit(const CVec2 &targCenter) override {}
  void UpdateDirection(const CVec2 &newDir) override;
  void UpdateDirection(WORD newDir) override;
  bool IsIdle() const override;
  bool IsTurning() const override { return false; }
  void StopUnit() override;
  void StopTurning() override {}
  void ForceGoByRightDir() override {}

  interface IStaticPathFinder *GetPathFinder() const override;
  // Is it possible to turn to the wNewDir direction? If not, then try to initialize the path to the point where a turn is possible
  bool CheckToTurn(const WORD wNewDir) override { return false; }

  void LockTiles(bool bUpdate = true) override {}
  void LockTilesForEditor() override {}
  void UnlockTiles(const bool bUpdate = true) override {}
  void FixUnlocking() override {}
  void UnfixUnlocking() override {}
  bool IsLockingTiles() const override { return false; }
  bool CanTurnToFrontDir(const WORD wDir) override { return false; }

  bool IsInFormation() const override { return false; }
  class CFormation *GetFormation() const override { return nullptr; }
  const CVec2 GetUnitPointInFormation() const override { return VNULL2; }
  const int GetFormationSlot() const override { return 0; }

  bool CanGoToPoint(const CVec2 &point) const override { return true; }
  BYTE GetAIClass() const override { return AI_CLASS_ANY; }

  float GetSmoothTurnThreshold() const override { return 1.0f; }

  void SetDesirableSpeed(float fDesirableSpeed) override;
  void UnsetDesirableSpeed() override;
  float GetDesirableSpeed() const override;
  void AdjustWithDesirableSpeed(float *pfMaxSpeed) const override;

  const int CanGoBackward() const override { return true; }
  bool HasSuspendedPoint() const override { return false; }
  bool CanRotateTo(SRect smallRect, const CVec2 &vNewDir, bool bWithUnits, bool bCanGoBackward = true) const override { return false; }

  bool IsInOneTrain(interface IBasePathUnit *pUnit) const override;
  bool IsTrain() const override { return true; }
  const CTrainPathUnit *GetTrainOwner() const { return this; }

  const SVector GetLastKnownGoodTile() const override { return GetTile(); }

  void LocomotiveDead();

  bool CanRotate() const override { return false; }

  void CarriageTrackDamaged(int nOwnerID, bool bTrackDamagedState);

  bool IsDangerousDirExist() const override { return false; }
  const WORD GetDangerousDir() const override { return 0; }
};

class CCarriagePathUnit : public CPathUnit
{
  OBJECT_COMPLETE_METHODS(CCarriagePathUnit);
  DECLARE_SERIALIZE;

  CObj<CTrainPathUnit> pTrain;

  CPtr<CEdgePoint> pFrontWheelPoint;
  CPtr<CEdgePoint> pBackWheelPoint;

  // the ribs on which the carriage stands
  std::list<CPtr<IEdge>> edges;
  CVec2 vOldDir;
  CVec2 vOldCenter;

  //
  void InitCenterAndDir3D(const CVec2 &vCenter, CVec3 *pvCenter3D, CVec3 *pvDir3D) const;
  const CVec3 Get3DPointOfUnit(const CVec2 &vCenter, float fLength) const;
  const CVec2 Get2DPointOfUnit(const CVec2 &vCenter, float fLength) const;

public:
  CCarriagePathUnit() {}

  void Init(class CAIUnit *pOwner, const CVec2 &center, int z, WORD dir, WORD id) override;
  const CTrainPathUnit *GetTrainOwner() const;

  ISmoothPath *GetSmoothPath() const override;

  interface ISmoothPath *GetCurPath() const override;
  void SetCurPath(interface ISmoothPath *pNewPath) override { NI_ASSERT_T(false, "Train can't change smooth path"); }
  void RestoreDefaultPath() override { NI_ASSERT_T(false, "Train can't change smooth path"); }

  void FirstSegment() override;
  void SecondSegment(bool bUpdate = true) override;
  IStaticPath *CreateBigStaticPath(const CVec2 &vStartPoint, const CVec2 &vFinishPoint, interface IPointChecking *pPointChecking) override;
  // returns whether I went or not
  bool SendAlongPath(interface IStaticPath *pStaticPath, const CVec2 &vShift, bool bSmoothTurn = true) override;
  bool SendAlongPath(interface IPath *pPath) override;

  void SetOnRailroad();
  void HookTo(CCarriagePathUnit *pUnit);

  const CVec3 GetBackHookPoint3D() const;
  const CVec2 GetBackHookPoint2D() const;
  const CVec3 GetFrontHookPoint3D() const;
  const CVec2 GetFronHookPoint2D() const;
  const CVec3 GetBackHookPoint3DByFrontPoint(const CVec2 &vFrontPoint) const;
  const CVec2 GetBackHookPoint2DByFrontPoint(const CVec2 &vFrontPoint) const;
  const CVec3 GetFrontWheel3D() const;
  const CVec2 GetFrontWheel2D() const;

  const CVec3 GetBackWheelPoint3DByFrontPoint(const CVec2 &vFrontPoint) const;
  const CVec2 GetBackWheelPoint2DByFrontPoint(const CVec2 &vFrontPoint) const;

  const float GetDistanceToBackWheel() const;
  const float GetDistanceBetweenWheels() const;

  void SetPlacementByWheels(CEdgePoint *pFrontWheelPoint, CEdgePoint *pBackWheelPoint);

  CEdgePoint *GetFrontWheelPoint() const;
  CEdgePoint *GetBackWheelPoint() const;

  void SetSpeed(const CVec2 &_speed) { speed = _speed; }

  void GetPlacement(struct SAINotifyPlacement *pPlacement, NTimer::STime timeDiff) const override;
  void SetRightDir(bool bRightDir) override;

  void SetNewCoordinates(const CVec3 &newCenter, bool bStopUnit = true) override;
  void SetNewCoordinatesForEditor(const CVec3 &newCenter) override;
  void SetCoordWOUpdate(const CVec3 &newCenter) override;

  bool CanTurnToFrontDir(const WORD wDir) override { return false; }
  virtual bool IsTrain() const { return true; }
  bool IsInOneTrain(interface IBasePathUnit *pUnit) const override;

  void UpdateDirectionForEditor(const CVec2 &dirVec) override;
  bool CanMove() const override;
  virtual bool CanMovePathfinding() const;
  bool CanRotate() const override { return false; }

  void UnitDead();

  void TrackDamagedState(bool bTrackDamaged) override;
};

#endif // __TRAIN_PATH_UNIT_H__