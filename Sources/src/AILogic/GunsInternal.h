#ifndef __GUNS_INTERNAL_H__
#define __GUNS_INTERNAL_H__

#pragma once

#include "Guns.h"

class CAIUnit;

// **********************************************************************
// *Gun barrels*
// **********************************************************************

class CGun : public CBasicGun
{
  DECLARE_SERIALIZE;

  enum EShootState { EST_TURNING, EST_AIMING, WAIT_FOR_ACTION_POINT, EST_SHOOTING, EST_REST };

  EShootState shootState;

  bool bWaitForReload;// specific for artillery

  // is it possible to fire a shot?
  bool bCanShoot;
  // how much is left in the queue
  int nShotsLast;

  CPtr<SCommonGunInfo> pCommonGunInfo;

  IGunsFactory::EGunTypes eType;
  EUnitAckType eRejectReason;

  CVec3 vLastShotPoint;

  float fRandom4Aim, fRandom4Relax;

  //

  void Aiming();
  void WaitForActionPoint();
  void Shooting();
  const CVec2 GetShootingPoint() const;
  WORD GetVisAngleOfAim() const;

  // to speed up shooting in good cases
  void OnWaitForActionPointState();
  void OnTurningState();
  void OnAimState();

protected:
  BYTE nShellType;
  CAIUnit *pOwner;
  int nOwnerParty;

  // the unit we are shooting at (in the case of shooting at a unit)
  CPtr<CAIUnit> pEnemy;
  // where to shoot
  CVec2 target;
  // time to start aiming or start rest, depending on the state
  NTimer::STime lastCheck;
  CVec2 lastEnemyPos;
  bool bAngleLocked;

  // do you need to aim?
  bool bAim;
  bool bGrenade;
  // height of the point at which the shooting is directed
  float z;

  using CParallelGuns = std::list<CPtr<CBasicGun>>;
  CParallelGuns parallelGuns;
  bool bParallelGun;
  NTimer::STime lastCheckTurnTime;

  const NTimer::STime GetActionPoint() const;
  //
  virtual bool TurnGunToEnemy(const CVec2 &vEnemyCenter, float zDiff) = 0;
  // Is it possible to shoot at the point right now (without rotating either the turret or the base), error - angle addAngle, cDeltaAngle - should deltaAngle be taken into account
  virtual bool IsGoodAngle(const CVec2 &point, WORD addAngle, float z, BYTE cDeltaAngle) const = 0;
  virtual void ToRestState();
  virtual void Rest() = 0;
  virtual bool AnalyzeTurning() = 0;
  // Is it possible to shoot at a target without rotating either the turret or the base and without moving?
  // cDeltaAngle - whether to take deltaAngle into account
  bool CanShootWOGunTurn(BYTE cDeltaAngle, float fZ);
  // Is it possible to shoot at a point if pUnit is inside a static. 
  bool AnalyzeLimitedAngle(class CCommonUnit *pUnit, const CVec2 &point) const;
  void Turning();
  bool CanShootToTargetWOMove();

  void InitRandoms();

public:
  CGun() : vLastShotPoint(VNULL3), pOwner(nullptr), bParallelGun(false), lastCheckTurnTime(0) {}
  CGun(class CAIUnit *pOwner, BYTE nShellType, SCommonGunInfo *pCommonGunInfo, IGunsFactory::EGunTypes eType);

  const float GetAimTime(bool bRandomize = true) const override;
  const float GetRelaxTime(bool bRandomize = true) const override;
  class CAIUnit *GetOwner() const override { return pOwner; }
  void SetOwner(CAIUnit *pOwner) override;
  bool IsAlive() const override;
  bool IsGrenade() const override { return bGrenade; }

  void GetMechShotInfo(SAINotifyMechShot *pMechShotInfo, const NTimer::STime &time) const override;
  void GetInfantryShotInfo(SAINotifyInfantryShot *pInfantryShotInfo, const NTimer::STime &time) const override;

  bool InFireRange(class CAIUnit *pTarget) const override;
  bool InFireRange(const CVec3 &vPoint) const override;
  float GetFireRange(float z) const override;
  // returns fRandgeMax from rpgstats taking into account all modifiers - coefficients
  float GetFireRangeMax() const override;
  bool InGoToSideRange(const class CAIUnit *pTarget) const override;
  bool TooCloseToFire(const class CAIUnit *pTarget) const override;
  bool TooCloseToFire(const CVec3 &vPoint) const override;

  void StartPointBurst(const CVec3 &target, bool bReAim) override;
  void StartPointBurst(const CVec2 &target, bool bReAim) override;
  void StartEnemyBurst(class CAIUnit *pEnemy, bool bReAim) override;
  void Segment() override;

  bool IsWaitForReload() const override { return bWaitForReload; }
  void ClearWaitForReload() override { bWaitForReload = false; }

  // currently in point and shoot mode
  bool IsFiring() const override;
  bool IsBursting() const override { return shootState == WAIT_FOR_ACTION_POINT || shootState == EST_SHOOTING; }

  const SBaseGunRPGStats &CGun::GetGun() const override;
  const SWeaponRPGStats *GetWeapon() const override;
  const SWeaponRPGStats::SShell &GetShell() const override;

  bool IsRelaxing() const override;
  // is it possible to shoot at pEnemy without rotating either the base or the turret, cDeltaAngle - should deltaAngle be taken into account
  bool CanShootWOGunTurn(class CAIUnit *pEnemy, BYTE cDeltaAngle) override;
  const NTimer::STime GetRestTimeOfRelax() const override;

  // shooting when moving is prohibited
  bool CanShootToUnitWOMove(class CAIUnit *pEnemy) override;
  bool CanShootToObjectWOMove(class CStaticObject *pObj) override;
  bool CanShootToPointWOMove(const CVec2 &point, float fZ, WORD wHorAddAngle = 0, WORD wVertAddAngle = 0, CAIUnit *pEnemy = nullptr) override;

  // Is it possible to shoot in height?
  bool CanShootByHeight(class CAIUnit *pTarget) const override;
  bool CanShootByHeight(float fZ) const override;

  // Is it possible to shoot at an object under a direct order?
  bool CanShootToUnit(class CAIUnit *pEnemy) override;
  bool CanShootToObject(class CStaticObject *pObj) override;
  bool CanShootToPoint(const CVec2 &point, float fZ, WORD wHorAddAngle = 0, WORD wVertAddAngle = 0) override;

  // you can shoot without rotating the base (the turret can be rotated)
  bool IsInShootCone(const CVec2 &point, WORD wAddAngle = 0) const override;

  const float GetDispersion() const override;
  const float GetDispRatio(byte nShellType, float fDist) const override;
  const int GetFireRate() const override;
  void LockInCurAngle() override { bAngleLocked = true; }
  void UnlockCurAngle() override { bAngleLocked = false; }

  // for aircraft
  void StartPlaneBurst(class CAIUnit *pEnemy, bool bReAim) override;

  // you can penetrate armor taking into account the side that pTarget is facing
  bool CanBreakArmor(class CAIUnit *pTarget) const override;
  // you can penetrate the armor from any side
  bool CanBreach(const class CCommonUnit *pTarget) const override;
  // you can penetrate the armor from the nSide side
  bool CanBreach(const class CCommonUnit *pTarget, int nSide) const override;
  bool CanBreach(const SHPObjectRPGStats *pStats, int nSide) const override;

  // will do all the actions necessary to shoot at a target (turning, aiming), but will not shoot
  void DontShoot() override { bCanShoot = false; }
  // cancels DontShoot()
  void CanShoot() override { bCanShoot = true; }
  bool IsShootAllowed() override { return bCanShoot; }

  // does the general gun fire (taking into account the cartridges - i.e. all guns that are in the same barrel with it are taken into account)
  bool IsCommonGunFiring() const override { return pCommonGunInfo->bFiring; }
  // Is pGun equal (taking into account cartridges)
  bool IsCommonEqual(const CBasicGun *pGun) const override;

  // "barrel number" (guns that differ only in cartridges, but are located in the same barrel)
  int GetCommonGunNumber() const override { return pCommonGunInfo->nGun; }

  int GetNAmmo() const override { return pCommonGunInfo->nAmmo; }

  interface IBallisticTraj *CreateTraj(const CVec2 &vTarget) const override;
  void Fire(const CVec2 &target, float z = 0) override;
  WORD GetTrajectoryZAngle(const CVec2 &vToAim, float z) const override;

  // tell me why he refused to shoot
  const EUnitAckType &GetRejectReason() const override { return eRejectReason; }
  void SetRejectReason(const EUnitAckType &eReason) override;

  const bool IsVisible(const BYTE party) const override { return true; }
  void GetTilesForVisibility(CTilesSet *pTiles) const override { pTiles->clear(); }
  bool ShouldSuspendAction(const EActionNotify &eAction) const override { return false; }

  void AddParallelGun(CBasicGun *pGun) override { parallelGuns.push_back(pGun); }
  void SetToParallelGun() override { bParallelGun = true; }

  // average value
  const int GetPiercing() const override;
  // spread
  const int GetPiercingRandom() const override;
  // random piercing value
  const int GetRandomPiercing() const override;
  const int GetMaxPossiblePiercing() const override;
  const int GetMinPossiblePiercing() const override;

  // average damage
  const float GetDamage() const override;
  // spread
  const float GetDamageRandom() const override;
  // random damage value
  const float GetRandomDamage() const override;

  bool IsBallisticTrajectory() const override;
};

// gun on turret
class CTurretGun : public CGun
{
  OBJECT_COMPLETE_METHODS(CTurretGun);
  DECLARE_SERIALIZE;

  WORD wBestWayDir;
  bool bTurnByBestWay;
  CPtr<CTurret> pTurret;
  bool bCircularAttack;

  //
  bool TurnByVer(const CVec2 &vEnemyCenter, float zDiff);
  bool TurnArtilleryToEnemy(const CVec2 &vEnemyCenter);
  bool TurnByBestWay(WORD wDirToEnemy);

  // this function calculates the angle at which the turret must be rotated in order to fire
  // given point. 
  WORD CalcVerticalAngle(const class CVec2 &pt, float z) const;

protected:
  bool TurnGunToEnemy(const CVec2 &vEnemyCenter, float zDiff) override;
  // Is it possible to shoot at the point right now (without rotating either the turret or the base), the error is the angle addAngle
  bool IsGoodAngle(const CVec2 &point, WORD addAngle, float z, BYTE cDeltaAngle) const override;
  void Rest() override;
  bool AnalyzeTurning() override;

public:
  CTurretGun() : bCircularAttack(false) {}
  CTurretGun(class CAIUnit *pOwner, BYTE nShellType, SCommonGunInfo *pCommonGunInfo, IGunsFactory::EGunTypes eType, int nTurret);

  bool IsOnTurret() const override { return true; }
  class CTurret *GetTurret() const override { return pTurret; }
  void TraceAim(class CAIUnit *pUnit) override;
  void StopTracing() override;

  void StopFire() override;

  // Is it possible to shoot in height?
  bool CanShootByHeight(class CAIUnit *pTarget) const override;

  // where is gun currently pointing?
  const WORD GetGlobalDir() const override;
  void TurnToRelativeDir(WORD wAngle) override;

  const float GetRotateSpeed() const override;

  WORD GetHorTurnConstraint() const override;
  WORD GetVerTurnConstraint() const override;

  void SetCircularAttack(bool bCanAttack) override;

  void StartPointBurst(const CVec3 &target, bool bReAim) override;
  void StartPointBurst(const CVec2 &target, bool bReAim) override;
  void StartEnemyBurst(class CAIUnit *pEnemy, bool bReAim) override;

  const NTimer::STime GetTimeToShootToPoint(const CVec3 &vPoint) const override;
  const NTimer::STime GetTimeToShoot(const CVec3 &vPoint) const override;
};

// cannon on base platform
class CBaseGun : public CGun
{
  OBJECT_COMPLETE_METHODS(CBaseGun);
  DECLARE_SERIALIZE;

protected:
  bool TurnGunToEnemy(const CVec2 &vEnemyCenter, float zDiff) override;
  // Is it possible to shoot at the point right now (without rotating either the turret or the base), the error is the angle addAngle
  bool IsGoodAngle(const CVec2 &point, WORD addAngle, float z, BYTE cDeltaAngle) const override;
  void Rest() override {}
  bool AnalyzeTurning() override;

public:
  CBaseGun() {}

  CBaseGun(class CAIUnit *pOwner, const BYTE nShellType, SCommonGunInfo *pCommonGunInfo, const IGunsFactory::EGunTypes eType)
    : CGun(pOwner, nShellType, pCommonGunInfo, eType) {}

  bool IsOnTurret() const override { return false; }
  class CTurret *GetTurret() const override { return nullptr; }
  void TraceAim(class CAIUnit *pUnit) override {}
  void StopTracing() override {}

  void StopFire() override;

  // where is gun currently pointing?
  const WORD GetGlobalDir() const override;
  void TurnToRelativeDir(const WORD wAngle) override {}

  const float GetRotateSpeed() const override;

  WORD GetHorTurnConstraint() const override { return 32768; }
  WORD GetVerTurnConstraint() const override { return 32768; }

  void SetCircularAttack(const bool bCanAttack) override {}
};

class CUnitsGunsFactory : public IGunsFactory
{
  class CAIUnit *pUnit;
  const int nCommonGun;
  int nTurret;

public:
  CUnitsGunsFactory(class CAIUnit *_pUnit, const int _nCommonGun, const int _nTurret)
    : pUnit(_pUnit), nCommonGun(_nCommonGun), nTurret(_nTurret) {}

  int GetNCommonGun() const override { return nCommonGun; }

  CBasicGun *CreateGun(const EGunTypes eType, const int nShell, SCommonGunInfo *pCommonGunInfo) const override
  {
    CBasicGun *pGun = nullptr;
    if (nTurret != -1) pGun = new CTurretGun(pUnit, nShell, pCommonGunInfo, eType, nTurret);
    else pGun = new CBaseGun(pUnit, nShell, pCommonGunInfo, eType);

    return pGun;
  }
};

#endif // __GUNS_INTERNAL_H__