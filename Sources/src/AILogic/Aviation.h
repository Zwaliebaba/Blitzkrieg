#ifndef __AVIATION_H__
#define __AVIATION_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "AIUnit.h"
#include "PlanePath.h"
// //////////////////////////////////////////////////////////// 
class CUnitGuns;
class CTurret;
// //////////////////////////////////////////////////////////// 
// class to extend if unit to go by CPlaneSmoothPath
interface IAviationUnit
{
  virtual const CVec2 &GetSpeedHorVer() const = 0;
  virtual void SetSpeedHorVer(const class CVec2 &_vSpeedHorVer) = 0;
  virtual const WORD GetDivingAngle() const = 0;
  virtual const WORD GetClimbingAngle() const = 0;
  virtual const CVec3 &GetNewPoint() const = 0;
};

// //////////////////////////////////////////////////////////// 
class CPlanesFormation : public IRefCount, public IAviationUnit, public IBasePathUnit
{
  OBJECT_COMPLETE_METHODS(CPlanesFormation);
  DECLARE_SERIALIZE;

  CPtr<CPlaneSmoothPath> pPath;
  CVec2 vCenter2D;
  float fZ;

  float fMaxSpeed;
  WORD wDirection;
  CVec2 vDirection;
  CVec2 vSpeedHorVer;

  // for member counting
  int nProcessed;
  int nAlive;

  CVec3 vNewPos;
  WORD wNewDirection;
  CVec2 vNewDirection;
  float fBombPointOffset;

  using CMemberCache = std::hash_map<CVec2, CPlaneSmoothPath::SMemberInfo, SVec2Hash, SVec2Equ>;
  CMemberCache memberCache;

public:
  void SetNewPos(const CVec3 &vCenter);
  CVec2 GetPointByFormationOffset(const CVec2 &vFormationOffset);
  WORD GetDirByFormationOffset(const CVec2 &vFormationOffset);
  float GetCurvatureRadius(const CVec2 &vFormationOffset);
  const CVec2 &GetSpeedByFormationOffset(const CVec2 &vFormationOffset);
  const float GetBombPointOffset() const { return fBombPointOffset; }

  void AddProcessed();
  void AddAlive();
  bool IsAllProcessed() const;
  void SecondSegment();
  const CVec2 &GetNewDirVector() const { return vNewDirection; }

  void Init(const CVec2 &vCenter, float _fZ, float fTurnRadiusMin, float fTurnRadiusMax, WORD _wDirection, float fMaxSpeed, float _fBombPointOffset);

  const CVec3 &GetNewPoint() const override { return vNewPos; }
  const CVec2 &GetSpeedHorVer() const override { return vSpeedHorVer; }
  void SetSpeedHorVer(const class CVec2 &_vSpeedHorVer) override { vSpeedHorVer = _vSpeedHorVer; }
  const WORD GetDivingAngle() const override { return 0; }
  const WORD GetClimbingAngle() const override { return 0; }


  void UpdateDirection(const CVec2 &newDir) override;
  void UpdateDirection(WORD newDir) override;

  bool SendAlongPath(interface IPath *pPath) override;
  const WORD GetDir() const override { return wDirection; }
  const CVec2 &GetDirVector() const override { return vDirection; }
  const WORD GetFrontDir() const override { return wDirection; }
  const CVec2 &GetCenter() const override { return vCenter2D; }
  const float GetZ() const override { return fZ; }

  const CVec2 &GetSpeed() const override
  {
    static CVec2 vSpeed;
    return (vSpeed = vDirection * fMaxSpeed);
  }

  interface ISmoothPath *GetCurPath() const override;

  // what an ugliness
  const WORD GetID() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return 0;
  }

  const float GetRotateSpeed() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return 0;
  }

  const float GetMaxSpeedHere(const CVec2 &point, bool bAdjust = true) const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return 0;
  }

  const float GetMaxPossibleSpeed() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return 0;
  }

  const float GetPassability() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return 0;
  }

  bool CanMove() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return true;
  }

  bool CanMovePathfinding() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return true;
  }

  const int GetBoundTileRadius() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return 1;
  }

  const CVec2 GetAABBHalfSize() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return VNULL2;
  }

  void SetCoordWOUpdate(const CVec3 &newCenter) override { NI_ASSERT_T(false, "WRONG CALL"); }
  void SetNewCoordinates(const CVec3 &newCenter, bool bStopUnit = true) override { NI_ASSERT_T(false, "WRONG CALL"); }

  const SRect GetUnitRectForLock() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return SRect();
  }

  bool TurnToDir(const WORD &newDir, const bool bCanBackward = true, const bool bForward = true) override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return true;
  }

  bool TurnToUnit(const CVec2 &targCenter) override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  bool IsIdle() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  bool IsTurning() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  void StopUnit() override {}
  void StopTurning() override { NI_ASSERT_T(false, "WRONG CALL"); }
  void ForceGoByRightDir() override { NI_ASSERT_T(false, "WRONG CALL"); }
  interface IStaticPathFinder *GetPathFinder() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return nullptr;
  }

  bool SendAlongPath(interface IStaticPath *pStaticPath, const CVec2 &vShift, bool bSmoothTurn = true) override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  bool CheckToTurn(const WORD wNewDir) override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  void LockTiles(bool bUpdate = true) override { NI_ASSERT_T(false, "WRONG CALL"); }
  void LockTilesForEditor() override { NI_ASSERT_T(false, "WRONG CALL"); }
  void UnlockTiles(const bool bUpdate = true) override { NI_ASSERT_T(false, "WRONG CALL"); }
  void FixUnlocking() override { NI_ASSERT_T(false, "WRONG CALL"); }
  void UnfixUnlocking() override { NI_ASSERT_T(false, "WRONG CALL"); }

  bool CanTurnToFrontDir(const WORD wDir) override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  bool IsInFormation() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  class CFormation *GetFormation() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return nullptr;
  }

  const CVec2 GetUnitPointInFormation() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return VNULL2;
  }

  const int GetFormationSlot() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return 0;
  }

  bool CanGoToPoint(const CVec2 &point) const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  float GetSmoothTurnThreshold() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return 0;
  }

  void SetDesirableSpeed(const float fDesirableSpeed) override { NI_ASSERT_T(false, "WRONG CALL"); }
  void UnsetDesirableSpeed() override { NI_ASSERT_T(false, "WRONG CALL"); }

  float GetDesirableSpeed() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return 0;
  }

  void AdjustWithDesirableSpeed(float *pfMaxSpeed) const override { NI_ASSERT_T(false, "WRONG CALL"); }

  const int CanGoBackward() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  bool IsLockingTiles() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  bool HasSuspendedPoint() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  bool CanRotateTo(SRect smallRect, const CVec2 &vNewDir, bool bWithUnits, bool bCanGoBackward = true) const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return true;
  }

  IStaticPath *CreateBigStaticPath(const CVec2 &vStartPoint, const CVec2 &vFinishPoint, interface IPointChecking *pPointChecking) override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return nullptr;
  }

  bool IsInOneTrain(interface IBasePathUnit *pUnit) const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  bool IsTrain() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return false;
  }

  const SVector GetLastKnownGoodTile() const override
  {
    NI_ASSERT_T(false, "WRONG CALL");
    return SVector(0, 0);
  }

  bool IsDangerousDirExist() const override { return false; }
  const WORD GetDangerousDir() const override { return 0; }
};

// //////////////////////////////////////////////////////////// 
class CAviation : public CAIUnit, public IAviationUnit
{
  OBJECT_NORMAL_METHODS(CAviation);
  DECLARE_SERIALIZE;

  CGDBPtr<SMechUnitRPGStats> pStats;

  // gun barrels
  CPtr<CUnitGuns> pGuns;

  // rotating cannon
  std::vector<CObj<CTurret>> turrets;

  // for aircraft formation
  CPtr<CPlanesFormation> pFormation;
  CVec2 vPlanesShift;// shift in formation

  CVec3 lastPos;
  WORD wLastDir;
  CVec3 vNormal, vFormerNormal;
  float fFormerCurvatureSign;

  float fTiltAnge;// turning angle
  NTimer::STime timeLastTilt;// last calculation of the angle of inclination

  CVec2 vSpeedHorVer;// decomposition of speed into horizontal and vertical components
  CVec2 vFormerHorVerSpeed;// for dive bombers
  CVec2 vFormerDir;

  int /* SUCAviation::AIRCRAFT_TYPE */ eAviationType;

protected:
  void InitGuns() override;
  const CUnitGuns *GetGuns() const override { return pGuns; }
  CUnitGuns *GetGuns() override { return pGuns; }

public:
  ~CAviation() override;

  int GetMovingType() const override;
  void Init(const CVec2 &center, int z, const SUnitBaseRPGStats *pStats, float fHP, WORD dir, BYTE player, WORD id, EObjVisType eVisType, int dbID) override;

  // IAviationUnit implementation
  // for vertical speed
  const CVec3 &GetNewPoint() const override
  {
    NI_ASSERT_T(false, "wrong call");
    return lastPos;
  }

  const CVec2 &GetSpeedHorVer() const override { return vSpeedHorVer; }
  void SetSpeedHorVer(const class CVec2 &_vSpeedHorVer) override { vSpeedHorVer = _vSpeedHorVer; }
  const WORD GetDivingAngle() const override;
  const WORD GetClimbingAngle() const override;

  // end IAviationUnit implementation

  void GetSpeed3(CVec3 *pSpeed) const override;

  void Segment() override;
  void SecondSegment(bool bUpdate = true) override;

  const SUnitBaseRPGStats *GetStats() const override { return pStats; }
  IStatesFactory *GetStatesFactory() const override;

  class CTurret *GetTurret(const int nTurret) const override { return turrets[nTurret]; }
  const int GetNTurrets() const override { return turrets.size(); }

  // for shooting
  void GetShotInfo(struct SAINotifyMechShot *pShotInfo) const override
  {
    pShotInfo->typeID = GetShootAction();
    pShotInfo->pObj = const_cast<CAviation *>(this);
  }

  const EActionNotify GetShootAction() const override { return ACTION_NOTIFY_MECH_SHOOT; }
  const EActionNotify GetAimAction() const override { return ACTION_NOTIFY_AIM; }
  const EActionNotify GetDieAction() const override { return ACTION_NOTIFY_DIE; }
  const EActionNotify GetIdleAction() const override { return ACTION_NOTIFY_IDLE; }
  const EActionNotify GetMovingAction() const override { return ACTION_NOTIFY_MOVE; }

  const bool CanShootToPlanes() const override;

  //
  int GetNGuns() const override;
  class CBasicGun *GetGun(int n) const override;

  class CBasicGun *ChooseGunForStatObj(class CStaticObject *pObj, NTimer::STime *pTime) override;
  float GetMaxFireRange() const override;

  const bool IsVisible(const BYTE party) const override { return true; }

  NTimer::STime GetDisappearInterval() const override { return 0; }

  // to obtain normals from fighters.
  float GetTerrainHeight(const float x, const float y, const NTimer::STime timeDiff) const override { return 0; }

  void GetPlacement(struct SAINotifyPlacement *pPlacement, NTimer::STime timeDiff) override;

  bool IsMech() const override { return true; }

  bool IsColliding() const override { return false; }

  const int CanGoBackward() const override { return false; }
  const float GetSightRadius() const override;
  void Die(bool fromExplosion, float fDamage) override;

  void StopUnit() override {}
  void Disappear() override;

  // lock unit (if it has already been locked, then the old lock disappears)
  void Lock(const CBasicGun *pGun) override {}
  // unlock unit (if locked by another gun, then nothing is done)
  void Unlock(const CBasicGun *pGun) override {}
  // is it locked by any gun that is not equal to pGun
  bool IsLocked(const CBasicGun *pGun) const override { return true; }

  // plane's formation, to force planes keep parade during flight.
  void SetPlanesFormation(class CPlanesFormation *pFormation, const CVec2 &vShift);
  CPlanesFormation *GetPlanesFormation();
  const CVec2 GetPlaneShift() const { return vPlanesShift; }

  const SRect GetUnitRect() const;
  const WORD GetDir() const override;
  const CVec2 &GetDirVector() const override;
  const WORD GetFrontDir() const override;
  const float GetZ() const override;
  const CVec2 &GetSpeed() const override;
  float GetPathCurvatureRadius() const;

  void SetAviationType(const int _eAviationType) { eAviationType = _eAviationType; }
  const int GetAviationType() const { return eAviationType; }

  const NTimer::STime GetNextSecondPathSegmTime() const override;
};

// //////////////////////////////////////////////////////////// 
#endif // __AVIATION_H__