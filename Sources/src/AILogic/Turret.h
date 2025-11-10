#ifndef __TURRET_H__
#define __TURRET_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "UpdatableObject.h"
#include "LinkObject.h"
// //////////////////////////////////////////////////////////// 
class CAIUnit;
class CBasicGun;

// //////////////////////////////////////////////////////////// 
class CTurret : public CLinkObject
{
  DECLARE_SERIALIZE;

  struct SRotating
  {
    // horizontal rotation speed
    float wRotationSpeed;
    // rel. angle 
    WORD wCurAngle, wFinalAngle;
    // which direction does it turn - + or -
    signed char sign;

    // the time when the turning process began and ended
    NTimer::STime startTime, endTime;

    // turn completed
    bool bFinished;
  };

  // horizontal aiming
  SRotating hor;
  // vertical aiming
  SRotating ver;

  // Is it possible to return the turret to the default rotation angle?
  bool bCanReturn;

  // is it aimed vertically?
  bool bVerAiming;

  CPtr<CAIUnit> pTracedUnit;
  CPtr<CBasicGun> pLockingGun;

  WORD wDefaultHorAngle;
  bool bReturnToNULLVerAngle;

  //
  WORD GetCurAngle(const SRotating &rotateInfo) const;
  void SetTurnParameters(SRotating *pRotateInfo, WORD wAngle, bool bInstantly);
  WORD ConstraintAngle(WORD wDesAngle, WORD wTurnConstraint) const;

public:
  CTurret() {}
  CTurret(WORD wHorRotationSpeed, WORD wVerRotationSpeed, bool bReturnToNULLVerAngle);

  const float GetHorRotationSpeed() const { return hor.wRotationSpeed; }

  virtual void Turn(WORD wHorAngle, WORD wVerAngle, bool bInstantly = false);
  // returns - a turn has been made, or the turret is already in the desired position
  virtual bool TurnHor(WORD wHorAngle, bool bInstantly = false);
  // returns - a turn has been made, or the turret is already in the desired position
  virtual bool TurnVer(WORD wVerAngle, bool bInstantly = false);

  void StopTurning();
  void StopHorTurning();
  void StopVerTurning();
  // is the turn completed?
  bool IsFinished() const { return hor.bFinished && ver.bFinished; }
  bool IsHorFinished() const { return hor.bFinished; }
  bool IsVerFinished() const { return ver.bFinished; }
  // you can return the gun to zero rotation angle
  void SetCanReturn();

  WORD GetHorCurAngle() const { return GetCurAngle(hor); }
  WORD GetVerCurAngle() const { return GetCurAngle(ver); }

  WORD GetHorFinalAngle() const { return hor.wFinalAngle; }
  WORD GetVerFinalAngle() const { return ver.wFinalAngle; }

  const NTimer::STime &GetHorEndTime() const { return hor.endTime; }
  const NTimer::STime &GetVerEndTime() const { return ver.endTime; }
  const NTimer::STime GetEndTurnTime() const { return Max(GetHorEndTime(), GetVerEndTime()); }

  void Segment();

  // accompany the enemy with a cannon
  void TraceAim(class CAIUnit *pUnit, class CBasicGun *pGun);
  class CAIUnit *GetTracedUnit() { return pTracedUnit; }
  void StopTracing();

  const float GetHorRotateSpeed() const { return hor.wRotationSpeed; }
  const float GetVerRotateSpeed() const { return ver.wRotationSpeed; }

  bool DoesRotateVert() const { return ver.wRotationSpeed != 0; }

  virtual CVec2 GetOwnerCenter() = 0;
  virtual WORD GetOwnerFrontDir() = 0;
  virtual float GetOwnerZ() = 0;
  virtual const int GetOwnerParty() const = 0;

  virtual WORD GetHorTurnConstraint() const = 0;
  virtual WORD GetVerTurnConstraint() const = 0;

  void GetHorTurretTurnInfo(struct SAINotifyTurretTurn *pTurretTurn) override = 0;
  void GetVerTurretTurnInfo(struct SAINotifyTurretTurn *pTurretTurn) override = 0;

  // lock with a turret gun (if it has already been locked, then the old lock disappears)
  void Lock(const class CBasicGun *pGun);
  // unlock turret (if locked with another gun, then nothing is done)
  void Unlock(const class CBasicGun *pGun);
  // is it locked by any gun that is not equal to pGun
  bool IsLocked(const class CBasicGun *pGun);

  void SetDefaultHorAngle(const WORD wHorAngle) { wDefaultHorAngle = wHorAngle; }
  const WORD &GetDefaultHorAngle() const { return wDefaultHorAngle; }

  virtual bool IsOwnerOperable() const = 0;
  bool IsAlive() const override = 0;

  const bool IsVisible(const BYTE cParty) const override { return true; }
  void GetTilesForVisibility(CTilesSet *pTiles) const override { pTiles->clear(); }
  bool ShouldSuspendAction(const EActionNotify &eAction) const override { return false; }

  // can/cannot be rotated
  virtual void SetRotateTurretState(bool bCanRotate) {}
  virtual bool GetRotateTurretState() const { return true; }
};

// //////////////////////////////////////////////////////////// 
class CUnitTurret : public CTurret
{
  OBJECT_COMPLETE_METHODS(CUnitTurret);
  DECLARE_SERIALIZE;

  CPtr<CAIUnit> pOwner;
  int nModelPart;
  DWORD dwGunCarriageParts;
  WORD wHorConstraint;
  WORD wVerConstraint;

  bool bCanRotateTurret;

public:
  CUnitTurret() {}
  CUnitTurret(class CAIUnit *pOwner, int nModelPart, DWORD dwGunCarriageParts, WORD wHorRotationSpeed, WORD wVerRotationSpeed, WORD wHorConstraint, WORD wVerConstraint);

  // returns - a turn has been made, or the turret is already in the desired position
  bool TurnHor(WORD wHorAngle, bool bInstantly = false) override;
  // returns - a turn has been made, or the turret is already in the desired position
  bool TurnVer(WORD wVerAngle, bool bInstantly = false) override;

  void GetHorTurretTurnInfo(struct SAINotifyTurretTurn *pTurretTurn) override;
  void GetVerTurretTurnInfo(struct SAINotifyTurretTurn *pTurretTurn) override;

  CVec2 GetOwnerCenter() override;
  WORD GetOwnerFrontDir() override;
  float GetOwnerZ() override;
  const int GetOwnerParty() const override;

  WORD GetHorTurnConstraint() const override;
  WORD GetVerTurnConstraint() const override { return wVerConstraint; }

  bool IsOwnerOperable() const override;
  bool IsAlive() const override;

  void SetRotateTurretState(bool bCanRotate) override { bCanRotateTurret = bCanRotate; }
  bool GetRotateTurretState() const override { return bCanRotateTurret; }
};

// //////////////////////////////////////////////////////////// 
class CMountedTurret : public CTurret
{
  OBJECT_COMPLETE_METHODS(CMountedTurret);
  DECLARE_SERIALIZE;

  class CBuilding *pBuilding;
  int nSlot;

  CVec2 center;
  WORD dir;
  WORD wHorTurnConstraint;
  WORD wVerTurnConstraint;

public:
  CMountedTurret() {}
  CMountedTurret(CBuilding *pBuliding, int nSlot);

  void GetHorTurretTurnInfo(struct SAINotifyTurretTurn *pTurretTurn) override {}
  void GetVerTurretTurnInfo(struct SAINotifyTurretTurn *pTurretTurn) override {}

  CVec2 GetOwnerCenter() override { return center; }
  WORD GetOwnerFrontDir() override { return dir; }
  float GetOwnerZ() override { return 0; }
  const int GetOwnerParty() const override;

  WORD GetHorTurnConstraint() const override;
  WORD GetVerTurnConstraint() const override;

  bool IsOwnerOperable() const override { return true; }
  bool IsAlive() const override;
};

// //////////////////////////////////////////////////////////// 
#endif // __TURRET_H__