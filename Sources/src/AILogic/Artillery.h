#ifndef __ARTILLERY_H__
#define __ARTILLERY_H__

#pragma once

#include "AIUnit.h"

class CUnitGuns;
class CTurret;
interface IPath;
class CFormation;
class CAIUnit;
class CArtilleryBulletStorage;
class CMechUnitGuns;

class CArtillery : public CAIUnit
{
  OBJECT_NORMAL_METHODS(CArtillery);
  DECLARE_SERIALIZE;

  CGDBPtr<SMechUnitRPGStats> pStats;

  int nInitialPlayer;

  // gun barrels
  CPtr<CMechUnitGuns> pGuns;

  // rotating cannon
  std::vector<CObj<CTurret>> turrets;

  EActionNotify eCurInstallAction, eNextInstallAction;
  // which install/uninstall are we currently in?
  EActionNotify eCurrentStateOfInstall;

  bool bInstalled;
  NTimer::STime installActionTime;
  bool bInstallActionInstant;// so that artillery can be created in an uninstalled state

  float fDispersionBonus;

  CPtr<IStaticPath> pStaticPathToSend;
  CVec2 vShift;
  CPtr<IPath> pIPathToSend;

  CPtr<CFormation> pCapturingUnit;// platoon, which runs to capture the cannon.
  CPtr<CFormation> pCrew;// the platoon that serves the gun
  float fOperable;// part of the team that maintains the cannon

  CPtr<CAIUnit> pSlaveTransport;// transport that works for this gun
  CPtr<CAIUnit> pHookingTransport;// transport, that is hooking this artillery.

  CObj<CArtilleryBulletStorage> pBulletStorage;
  bool bBulletStorageVisible;
  NTimer::STime lastCheckToInstall;

  NTimer::STime behUpdateDuration;

  // creates an ammo box for AI, but does not send it for visualization
  void CreateAmmoBox();

  // can the ammo box be seen by the player?
  void ShowAmmoBox();
  void HideAmmoBox();

  //
  bool IsInstallActionFinished();
  bool ShouldSendInstallAction(const EActionNotify &eAction) const;

protected:
  void InitGuns() override;
  const CUnitGuns *GetGuns() const override;
  CUnitGuns *GetGuns() override;

public:
  CArtillery() : bBulletStorageVisible(false) {}
  ~CArtillery() override;
  void Init(const CVec2 &center, int z, const SUnitBaseRPGStats *pStats, float fHP, WORD _dir, BYTE player, WORD id, EObjVisType eVisType, int dbID) override;

  const SUnitBaseRPGStats *GetStats() const override { return pStats; }
  IStatesFactory *GetStatesFactory() const override;

  void Segment() override;
  void SetSelectable(bool bSelectable) override;

  void SetCamoulfage() override;
  void RemoveCamouflage(ECamouflageRemoveReason eReason) override;

  class CTurret *GetTurret(const int nTurret) const override { return turrets[nTurret]; }
  const int GetNTurrets() const override { return turrets.size(); }

  void GetShotInfo(struct SAINotifyMechShot *pShotInfo) const override
  {
    pShotInfo->typeID = GetShootAction();
    pShotInfo->pObj = const_cast<CArtillery *>(this);
  }

  const EActionNotify GetShootAction() const override { return ACTION_NOTIFY_MECH_SHOOT; }
  const EActionNotify GetAimAction() const override { return ACTION_NOTIFY_AIM; }
  const EActionNotify GetDieAction() const override { return ACTION_NOTIFY_DIE; }
  const EActionNotify GetIdleAction() const override { return ACTION_NOTIFY_IDLE; }
  const EActionNotify GetMovingAction() const override { return ACTION_NOTIFY_MOVE; }

  //
  int GetNGuns() const override;
  class CBasicGun *GetGun(int n) const override;

  class CBasicGun *ChooseGunForStatObj(class CStaticObject *pObj, NTimer::STime *pTime) override;

  bool IsInstalled() const override { return bInstalled && eCurInstallAction == ACTION_NOTIFY_NONE && eNextInstallAction == ACTION_NOTIFY_NONE; }
  bool IsUninstalled() const override { return !bInstalled && eCurInstallAction == ACTION_NOTIFY_NONE && eNextInstallAction == ACTION_NOTIFY_NONE; }
  bool IsInInstallAction() const { return eCurInstallAction != ACTION_NOTIFY_NONE || eNextInstallAction != ACTION_NOTIFY_NONE; }

  void InstallBack(bool bAlreadyDone);// install the artillery back, if it is not uninstalled, then it’s an error.
  void InstallAction(EActionNotify eInstallAction, bool bAlreadyDone = false) override;
  // install right now
  void ForceInstallAction();

  const bool NeedDeinstall() const override;
  const bool CanShoot() const override { return IsInstalled(); }
  class CBasicGun *GetFirstArtilleryGun() const override;
  void TakeDamage(float fDamage, const SWeaponRPGStats::SShell *pShell, int nPlayerOfShoot, CAIUnit *pShotUnit) override;

  bool IsLightGun() const;
  const bool CanShootToPlanes() const override;

  bool IsIdle() const override;
  bool SendAlongPath(interface IStaticPath *pStaticPath, const CVec2 &vShift, bool bSmoothTurn = true) override;
  bool SendAlongPath(IPath *pPath) override;

  float GetMaxFireRange() const override;
  void GetRangeArea(struct SShootAreas *pRangeArea) const override;

  // bonuses
  const float GetDispersionBonus() const override;
  const void SetDispersionBonus(const float fBonus) override { fDispersionBonus = fBonus; }

  bool IsMech() const override { return true; }

  bool TurnToDir(const WORD &newDir, bool bCanBackward = true, bool bForward = true) override;
  bool TurnToUnit(const CVec2 &targCenter) override;

  // gun maintenance by artillerymen
  void ChangePlayer(BYTE cPlayer) override;
  virtual void SetCrew(class CFormation *_pCrew, bool bCapture = true);
  virtual void DelCrew();
  virtual bool HasServeCrew() const;
  virtual bool MustHaveCrewToOperate() const;
  virtual class CFormation *GetCrew() const;
  bool IsOperable() const override { return fOperable != 0.0f; }
  virtual void SetOperable(float fOperable);

  void Disappear() override;

  // for towing
  virtual CVec2 GetTowPoint();

  // CRAP { to milestonestone patch
  virtual void SetSlaveTransport(class CAIUnit *_pSlaveTransport) { pSlaveTransport = _pSlaveTransport; }
  virtual bool HasSlaveTransport();
  virtual class CAIUnit *GetSlaveTransport() { return pSlaveTransport; }
  // CRAP}

  const CVec2 GetAmmoBoxCoordinates();

  const float GetMaxSpeedHere(const CVec2 &point, bool bAdjust = true) const override;
  const float GetRotateSpeed() const override;

  // allows/prohibits all Guns for a given gun to fire
  virtual void DoAllowShoot(bool allow);

  // clears the flag for all guns to wait for reloading
  virtual void ClearWaitForReload();

  bool IsColliding() const override;

  const int CanGoBackward() const override { return GetCrew() == nullptr; }

  void StopUnit() override;
  const DWORD GetNormale(const CVec2 &vCenter) const override;
  const DWORD GetNormale() const override;

  const CVec2 GetHookPoint() const;
  const CVec3 GetHookPoint3D() const;

  EActionNotify GetCurUninstallAction() const { return eCurrentStateOfInstall; }

  void LookForTarget(CAIUnit *pCurTarget, bool bDamageUpdated, CAIUnit **pBestTarget, class CBasicGun **pGun) override;

  void SendAcknowledgement(CAICommand *pCommand, EUnitAckType ack, bool bForce = false) override;
  void SendAcknowledgement(EUnitAckType ack, bool bForce = false) override;

  int GetInitialPlayer() const { return nInitialPlayer; }
  void SetInitialPlayer(const int nPlayer) { nInitialPlayer = nPlayer; }
  const NTimer::STime &GetBehUpdateDuration() const override;
  //
  bool IsBeingCaptured() const;
  void SetCapturingUnit(CFormation *pFormation);
  CFormation *GetCapturedUnit() { return pCapturingUnit; }

  // to allow only one transport to hook artillery
  bool IsBeingHooked() const;
  void SetBeingHooked(class CAIUnit *pUnit);
  CAIUnit *GetHookingTransport();

  void UpdateAmmoBoxVisibility();
};

#endif // __ARTILLERY_H__