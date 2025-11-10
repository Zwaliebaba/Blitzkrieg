#ifndef __GUNS_H__
#define __GUNS_H__

#pragma once

#include "UpdatableObject.h"
#include "LinkObject.h"

// **********************************************************************
// *Unit Weapon*
// **********************************************************************

struct SCommonGunInfo : IRefCount
{
  OBJECT_COMPLETE_METHODS(SCommonGunInfo);
  DECLARE_SERIALIZE;

public:
  bool bFiring;
  int nAmmo;
  NTimer::STime lastShoot;
  int nGun;

  SCommonGunInfo() {}
  SCommonGunInfo(bool _bFiring, const int _nAmmo, const int _nGun) : bFiring(_bFiring), nAmmo(_nAmmo), lastShoot(0), nGun(_nGun) {}
};

interface IGunsFactory
{
  enum EGunTypes { MOMENT_CML_GUN, MOMENT_BURST_GUN, VIS_CML_BALLIST_GUN, VIS_BURST_BALLIST_GUN, PLANE_GUN, MORALE_GUN };

  virtual class CBasicGun *CreateGun(EGunTypes eType, int nShell, SCommonGunInfo *pCommonGunInfo) const = 0;
  virtual int GetNCommonGun() const = 0;
};

class CBasicGun : public CLinkObject
{
  DECLARE_SERIALIZE;

public:
  virtual const float GetAimTime(bool bRandomize = true) const = 0;
  virtual const float GetRelaxTime(bool bRandomize = true) const = 0;
  virtual class CAIUnit *GetOwner() const = 0;
  virtual void SetOwner(class CAIUnit *pUnit) = 0;
  bool IsAlive() const override = 0;
  virtual bool IsGrenade() const = 0;

  void GetMechShotInfo(SAINotifyMechShot *pMechShotInfo, const NTimer::STime &time) const override = 0;
  void GetInfantryShotInfo(SAINotifyInfantryShot *pInfantryShotInfo, const NTimer::STime &time) const override = 0;

  virtual bool InFireRange(class CAIUnit *pTarget) const = 0;
  virtual bool InFireRange(const CVec3 &vPoint) const = 0;
  virtual float GetFireRange(float z) const = 0;
  // returns fRandgeMax from rpgstats taking into account all modifiers - coefficients
  virtual float GetFireRangeMax() const = 0;
  virtual bool InGoToSideRange(const class CAIUnit *pTarget) const = 0;
  virtual bool TooCloseToFire(const class CAIUnit *pTarget) const = 0;
  virtual bool TooCloseToFire(const CVec3 &vPoint) const = 0;

  virtual void StartPointBurst(const CVec3 &_target, bool bReAim) = 0;
  virtual void StartPointBurst(const CVec2 &target, bool bReAim) = 0;
  virtual void StartEnemyBurst(class CAIUnit *pEnemy, bool bReAim) = 0;
  virtual void Segment() = 0;

  // currently in point and shoot mode
  virtual bool IsFiring() const = 0;
  // there's a burst of fire at the moment
  virtual bool IsBursting() const = 0;
  virtual void StopFire() = 0;

  // begin artillery specific
  virtual bool IsWaitForReload() const =0;
  virtual void ClearWaitForReload() =0;
  // end artillery specific

  virtual void Fire(const CVec2 &target, float z = 0) = 0;
  virtual interface IBallisticTraj *CreateTraj(const CVec2 &vTarget) const = 0;

  virtual const SBaseGunRPGStats &GetGun() const = 0;
  virtual const SWeaponRPGStats *GetWeapon() const = 0;
  virtual const SWeaponRPGStats::SShell &GetShell() const = 0;
  virtual bool IsOnTurret() const = 0;

  virtual void TraceAim(class CAIUnit *pUnit) = 0;
  virtual void StopTracing() = 0;
  virtual bool IsRelaxing() const = 0;
  // Is it possible to shoot at a target without rotating either the turret or the base and without moving, cDeltaAngle - should deltaAngle be taken into account
  virtual bool CanShootWOGunTurn(class CAIUnit *pEnemy, BYTE cDeltaAngle) = 0;
  // the direction in which the gun is currently facing
  virtual const WORD GetGlobalDir() const = 0;
  // if on a turret, then rotate to the relative angle wAngle
  virtual void TurnToRelativeDir(WORD wAngle) = 0;

  // how much more relax do you need?
  virtual const NTimer::STime GetRestTimeOfRelax() const = 0;
  virtual const float GetRotateSpeed() const = 0;

  // shooting when moving is prohibited, but you can rotate (if the unit can rotate)
  virtual bool CanShootToUnitWOMove(class CAIUnit *pEnemy) = 0;
  virtual bool CanShootToObjectWOMove(class CStaticObject *pObj) = 0;
  virtual bool CanShootToPointWOMove(const CVec2 &point, float fZ, WORD wHorAddAngle = 0, WORD wVertAddAngle = 0, CAIUnit *pEnemy = nullptr) = 0;

  // Is it possible to shoot at an object under a direct order (taking into account whether the owner can drive or not)
  virtual bool CanShootToUnit(class CAIUnit *pEnemy) = 0;
  virtual bool CanShootToObject(class CStaticObject *pObj) = 0;
  // Is it possible to shoot at an object under a direct order (you can’t drive up)
  virtual bool CanShootToPoint(const CVec2 &point, float fZ, WORD wHorAddAngle = 0, WORD wVertAddAngle = 0) = 0;
  // Is it possible to shoot in height?
  virtual bool CanShootByHeight(CAIUnit *pTarget) const = 0;
  virtual bool CanShootByHeight(float fZ) const = 0;

  // for aircraft
  virtual void StartPlaneBurst(class CAIUnit *pEnemy, bool bReAim) = 0;

  // you can shoot without rotating the base (the turret can be rotated)
  virtual bool IsInShootCone(const CVec2 &point, WORD wAddAngle = 0) const = 0;

  virtual const float GetDispersion() const = 0;
  // the scatter along the trajectory is so many times greater than the scatter across the trajectory
  virtual const float GetDispRatio(byte nShellType, float fDist) const =0;
  virtual const int GetFireRate() const = 0;
  virtual void LockInCurAngle() = 0;
  virtual void UnlockCurAngle() = 0;

  virtual WORD GetHorTurnConstraint() const = 0;
  virtual WORD GetVerTurnConstraint() const = 0;
  virtual class CTurret *GetTurret() const = 0;

  // you can penetrate armor taking into account the side that pTarget is facing
  virtual bool CanBreakArmor(class CAIUnit *pTarget) const = 0;
  // you can penetrate the armor from any side
  virtual bool CanBreach(const class CCommonUnit *pTarget) const = 0;
  // you can penetrate the armor from the nSide side
  virtual bool CanBreach(const class CCommonUnit *pTarget, int nSide) const = 0;
  virtual bool CanBreach(const SHPObjectRPGStats *pStats, int nSide) const = 0;

  // will do all the actions necessary to shoot at a target (turning, aiming), but will not shoot
  virtual void DontShoot() = 0;
  // cancels DontShoot()
  virtual void CanShoot() = 0;
  virtual bool IsShootAllowed() =0;

  // does the general gun fire (taking into account the cartridges - i.e. all guns that are in the same barrel with it are taken into account)
  virtual bool IsCommonGunFiring() const = 0;
  // Is pGun equal (taking into account cartridges)
  virtual bool IsCommonEqual(const CBasicGun *pGun) const = 0;

  // "barrel number" (guns that differ only in cartridges, but are located in the same barrel)
  virtual int GetCommonGunNumber() const = 0;

  virtual int GetNAmmo() const = 0;

  virtual WORD GetTrajectoryZAngle(const CVec2 &vToAim, float z) const = 0;

  // tell me why he refused to shoot
  virtual const EUnitAckType &GetRejectReason() const = 0;
  virtual void SetRejectReason(const EUnitAckType &eRejectReason) = 0;

  // allow/prohibit attacks without taking into account the restriction on turning the gun horizontally
  virtual void SetCircularAttack(bool bCanAttack) = 0;

  virtual void AddParallelGun(CBasicGun *pGun) = 0;
  virtual void SetToParallelGun() = 0;

  // average value
  virtual const int GetPiercing() const = 0;
  // spread
  virtual const int GetPiercingRandom() const = 0;
  // random piercing value
  virtual const int GetRandomPiercing() const = 0;
  virtual const int GetMaxPossiblePiercing() const = 0;
  virtual const int GetMinPossiblePiercing() const = 0;

  // average damage
  virtual const float GetDamage() const = 0;
  // spread
  virtual const float GetDamageRandom() const = 0;
  // random damage value
  virtual const float GetRandomDamage() const = 0;

  // for AA guns.
  virtual const NTimer::STime GetTimeToShootToPoint(const CVec3 &vPoint) const
  {
    NI_ASSERT_T(false, "wrong call");
    return 0;
  }

  virtual const NTimer::STime GetTimeToShoot(const CVec3 &vPoint) const
  {
    NI_ASSERT_T(false, "wrong call");
    return 0;
  }

  virtual bool IsBallisticTrajectory() const = 0;
};

float GetDispByRadius(const class CBasicGun *pGun, const CVec2 &attackerPos, const CVec2 &explCoord);
float GetDispByRadius(const class CBasicGun *pGun, float fDist);
float GetDispByRadius(float fDispRadius, float fRangeMax, float fDist);

const float GetFireRangeMax(const SWeaponRPGStats *pStats, CAIUnit *pOwner);

#endif // __GUNS_H__