#ifndef __AI_UNIT_H__
#define __AI_UNIT_H__
// //////////////////////////////////////////////////////////// 
#pragma once
// //////////////////////////////////////////////////////////// 
#include "CommonUnit.h"
#include "AnimUnit.h"

class CPathUnit;
class CAntiArtillery;
class CAIUnitInfoForGeneral;
class CExistingObject;

class CAIUnit : public CCommonUnit
{
  DECLARE_SERIALIZE;

  WORD id;

  BYTE player;

  // half viewing angle
  WORD wVisionAngle;
  // vision multiplier
  float fSightMultiplier;

  bool bUnitUnderSupply;

  // RPG stats
  float fHitPoints;

  CObj<CPathUnit> pPathUnit;
  CObj<CAntiArtillery> pAntiArtillery;
  CObj<IAnimUnit> pAnimUnit;

  NTimer::STime timeLastmoraleUpdate;

  float fMorale;
  bool bHasMoraleSupport;// next to this unit there is a unit that supports morale

  NTimer::STime lastTimeOfVis;

  // the trench in which this unit hangs out
  CPtr<CExistingObject> pTankPit;

  NTimer::STime camouflateTime;

  bool bVisibleByPlayer;
  WORD wWisibility;// informatin about visibility for every player
  NTimer::STime lastAckTime;
  std::vector<int> visible4Party;

  float fTakenDamagePower;
  int nGrenades;
  NTimer::STime targetScanRandom;

  float fExperience;
  int nLevel;
  CGDBPtr<SAIExpLevel> pExpLevels;

  bool bFreeEnemySearch;
  NTimer::STime creationTime;

  bool bAlwaysVisible;

  bool bRevealed;
  bool bQueredToReveal;
  NTimer::STime nextRevealCheck;
  CVec2 vPlaceOfReveal;
  NTimer::STime timeOfReveal;
  int nVisIndexInUnits;

  //
  const SAINotifyHitInfo::EHitType ProcessExactHit(const SRect &combatRect, const CVec2 &explCoord, int nRandPiercing, int nRandArmor) const;
  // sends to general request according to units's current state; 
  void UpdateUnitsRequestsForResupply();
  void UpdateTankPitVisibility();
  void InitializeShootArea(struct SShootArea *pArea, CBasicGun *pGun, float fRangeMin, float fRangeMax) const;
  void CheckForReveal();

protected:
  NTimer::STime timeToDeath;

  CObj<CAIUnitInfoForGeneral> pUnitInfoForGeneral;

  bool bAlive;
  float fCamouflage;
  // necessary actions before death/disappearance from the map
  virtual void PrepareToDelete();
  void DieTrain(float fDamage);

  virtual void InitGuns() = 0;
  void Init(const CVec2 &center, int z, float fHP, WORD dir, BYTE player, WORD id, EObjVisType eVisType, int dbID);

  const int GetRandArmorByDir(int nArmorDir, WORD wAttackDir, const SRect &unitRect);

  bool IsTimeToAnalyzeTargetScan() const;
  void SetMoraleForced(float _fMorale);

  virtual bool CalculateUnitVisibility4Party(BYTE cParty) const;

public:
  virtual void Init(const CVec2 &center, int z, const SUnitBaseRPGStats *pStats, float fHP, WORD dir, BYTE player, WORD id, EObjVisType eVisType, int dbID) = 0;
  void SetScenarioStats();

  void SetRightDir(bool _bRightDir) override;
  // for updater
  bool IsAlive() const override { return bAlive; }
  void GetNewUnitInfo(struct SNewUnitInfo *pNewUnitInfo) override;
  void GetRPGStats(struct SAINotifyRPGStats *pStats) override;
  void GetPlacement(struct SAINotifyPlacement *pPlacement, NTimer::STime timeDiff) override;
  void GetSpeed3(CVec3 *pSpeed) const override;
  const NTimer::STime GetTimeOfDeath() const override { return timeToDeath; }

  // when a tank has entered the TankPit or is dug in, this function is called
  void SetInTankPit(CExistingObject *pTankPit);
  void SetOffTankPit();
  class CExistingObject *GetTankPit() const { return pTankPit; }
  bool IsInTankPit() const;

  void SetVisionAngle(const WORD wAngle) { wVisionAngle = wAngle; }
  WORD GetVisionAngle() const { return wVisionAngle; }
  void SetSightMultiplier(const float _fSightMultiplier) { fSightMultiplier = _fSightMultiplier; }
  virtual float GetSightMultiplier() const { return fSightMultiplier; }

  //
  const WORD GetID() const override { return id; }
  virtual const SUnitBaseRPGStats *GetStats() const = 0;

  // in a place where no one sees him and he sees no one
  virtual bool IsInSolidPlace() const { return false; }
  virtual bool IsInFirePlace() const { return false; }
  bool IsFree() const override { return true; }

  // command processing
  void Segment() override;
  void FreezeSegment() override;
  bool IsPossibleChangeAction() const { return bAlive; }
  void Die(bool fromExplosion, float fDamage) override;
  void Disappear() override;

  // visible to this diplomatic party
  const bool IsVisible(BYTE party) const override;
  // for deferred updates
  void GetTilesForVisibility(CTilesSet *pTiles) const override;
  bool ShouldSuspendAction(const EActionNotify &eAction) const override;
  virtual const DWORD GetNormale(const CVec2 &vCenter) const;
  virtual const DWORD GetNormale() const;

  // IBasePathUnit
  const CVec2 &GetCenter() const override;
  const SVector GetTile() const override;
  const float GetRotateSpeed() const override;
  const float GetMaxPossibleSpeed() const override;
  const float GetPassability() const override;
  const CVec2 &GetSpeed() const override;
  virtual float GetSpeedLen() const;
  const int GetBoundTileRadius() const override;
  const WORD GetDir() const override;
  const WORD GetFrontDir() const override;
  interface IStaticPath *CreateBigStaticPath(const CVec2 &vStartPoint, const CVec2 &vFinishPoint, interface IPointChecking *pPointChecking) override;

  const CVec2 &GetDirVector() const override;
  const CVec2 GetAABBHalfSize() const override;
  void SetCoordWOUpdate(const CVec3 &newCenter) override;
  void SetNewCoordinates(const CVec3 &newCenter, bool bStopUnit = true) override;
  virtual void SetNewCoordinatesForEditor(const CVec3 &newCenter);
  bool CanSetNewCoord(const CVec3 &newCenter) const;
  const SRect GetUnitRectForLock() const override;
  bool TurnToDir(const WORD &newDir, bool bCanBackward = true, bool bForward = true) override;
  void UpdateDirection(const CVec2 &newDir) override;
  virtual void UpdateDirectionForEditor(const CVec2 &newDir);
  void UpdateDirection(WORD newDir) override;
  bool CanSetNewDir(const CVec2 &newDir) const;
  bool IsIdle() const override;
  bool IsTurning() const override;
  void StopUnit() override;
  void StopTurning() override;
  void ForceGoByRightDir() override;
  interface IStaticPathFinder *GetPathFinder() const override;

  interface ISmoothPath *GetCurPath() const override;
  virtual void SetCurPath(interface ISmoothPath *pNewPath);
  virtual void RestoreDefaultPath();

  bool SendAlongPath(IStaticPath *pStaticPath, const CVec2 &vShift, bool bSmoothTurn = true) override;
  bool SendAlongPath(IPath *pPath) override;
  void LockTiles(bool bUpdate = true) override;
  void ForceLockingTiles(bool bUpdate = true);
  void LockTilesForEditor() override;
  void UnlockTiles(bool bUpdate = true) override;
  const float GetZ() const override;
  const WORD GetDirAtTheBeginning() const;

  // CPathUnit
  const SRect GetUnitRect() const;
  virtual void FirstSegment();
  virtual void SecondSegment(bool bUpdate = true);
  void FixUnlocking() override;
  void UnfixUnlocking() override;
  bool IsInOneTrain(interface IBasePathUnit *pUnit) const override;
  bool IsTrain() const override;

  //
  const float GetHitPoints() const { return fHitPoints; }
  void IncreaseHitPoints(float fInc = 1);
  // from the editor
  void TakeEditorDamage(float fDamage);
  virtual void TakeDamage(float fDamage, const SWeaponRPGStats::SShell *pShell, int nPlayerOfShoot, CAIUnit *pShotUnit);
  // true on hit
  virtual bool ProcessCumulativeExpl(class CExplosion *pExpl, int nArmorDir, bool bFromExpl);
  // true for an exact hit
  virtual bool ProcessBurstExpl(class CExplosion *pExpl, int nArmorDir, float fRadius, float fSmallRadius);
  // true on hit
  virtual bool ProcessAreaDamage(const class CExplosion *pExpl, int nArmorDir, float fRadius, float fSmallRadius);

  // the probability with which damage will be caused when hit
  virtual const float GetCover() const;
  virtual bool IsSavedByCover() const;

  class CTurret *GetTurret(const int nTurret) const override
  {
    NI_ASSERT_T(false, "Wrong call of get turret");
    return nullptr;
  }

  // for shooting
  virtual void GetShotInfo(struct SAINotifyInfantryShot *pShotInfo) const { NI_ASSERT_T(false, "Wrong call of GetShotInfo"); }
  virtual void GetShotInfo(struct SAINotifyMechShot *pShotInfo) const { NI_ASSERT_T(false, "Wrong call of GetShotInfo"); }
  virtual const EActionNotify GetShootAction() const = 0;
  virtual const EActionNotify GetAimAction() const = 0;
  const EActionNotify GetDieAction() const override = 0;
  virtual const EActionNotify GetIdleAction() const = 0;
  // there are types for Move
  virtual const EActionNotify GetMovingAction() const = 0;
  int GetMovingType() const override { return 0; }

  bool CanMove() const override;
  bool CanMovePathfinding() const override;
  // can it turn around in principle (uninstallation may be necessary)
  bool CanRotate() const override;

  void SetCamoulfage() override;
  const float GetCamouflage() const { return fCamouflage; }
  void RemoveCamouflage(ECamouflageRemoveReason eReason) override;
  virtual bool IsCamoulflated() const { return fCamouflage < 1.0f; }

  bool CanCommandBeExecuted(class CAICommand *pCommand) override;
  bool CanCommandBeExecutedByStats(class CAICommand *pCommand) override;
  bool CanCommandBeExecutedByStats(int nCmd) const override;

  const BYTE GetPlayer() const override { return player; }
  void SetPlayerForEditor(int nPlayer) override;
  // change diplomacy with the correct update in units
  void ChangePlayer(BYTE cPlayer) override;
  // just put another diplomacy
  void SetPlayer(const BYTE cPlayer) { player = cPlayer; }

  virtual bool InVisCone(const CVec2 &point) const { return true; }
  const float GetSightRadius() const override;
  float GetRemissiveCoeff() const;
  const int GetNAIGroup() const override { return GetNGroup(); }

  class CPathUnit *GetPathUnit() const { return pPathUnit; }

  bool CanTurnToFrontDir(WORD wDir) override;

  const NTimer::STime GetTimeToCamouflage() const override;
  virtual void AnalyzeCamouflage();
  virtual void StartCamouflating();


  void CreateAntiArtillery(float fMaxRevealRadius);
  void Fired(float fGunRadius, int nGun) override;
  virtual NTimer::STime GetDisappearInterval() const { return SConsts::TIME_TO_DISAPPEAR; }

  // bonuses
  virtual const float GetDispersionBonus() const { return 1.0f; }
  virtual const void SetDispersionBonus(const float fBonus) {}
  virtual const float GetRelaxTimeBonus() const { return 1.0f; }
  virtual const float GetFireRateBonus() const { return 1.0f; }
  virtual const float GetAimTimeBonus() const { return 1.0f; }

  void SetAmbush() override;
  void RemoveAmbush() override;

  virtual void GetFogInfo(struct SFogInfo *pInfo) const;
  void GetShootAreas(struct SShootAreas *pShootAreas, int *pnAreas) const override;
  void WarFogChanged();

  // update images shoot area or range area
  void UpdateArea(EActionNotify eAction) override;

  BYTE GetAIClass() const override { return GetStats()->aiClass; }

  // cartridges
  const int GetNCommonGuns() const;
  const SBaseGunRPGStats &GetCommonGunStats(int nCommonGun) const;
  virtual int GetNAmmo(int nCommonGun) const;
  // nAmmo with sign
  virtual void ChangeAmmo(int nCommonGun, int nAmmo);
  virtual bool IsCommonGunFiring(int nCommonGun) const;

  // repair cost 1 HP

  void SetMorale(float _fMorale);
  void SetMoraleSupport();
  const float GetMorale() const { return fMorale; }

  float GetSmoothTurnThreshold() const override;

  // whether the unit is noticeable pUnit, detection radius fNoticeRadius
  virtual bool IsNoticableByUnit(class CCommonUnit *pUnit, float fNoticeRadius);

  const int ChooseFatality(float fDamage);

  void NullCollisions();

  void SendAcknowledgement(EUnitAckType ack, bool bForce = false) override;
  // ack for pCommand
  void SendAcknowledgement(CAICommand *pCommand, EUnitAckType ack, bool bForce = false) override;

  // does it collide with moving objects?
  virtual bool IsColliding() const;

  const int GetMinArmor() const override;
  const int GetMaxArmor() const override;
  const int GetMinPossibleArmor(int nSide) const override;
  const int GetMaxPossibleArmor(int nSide) const override;
  const int GetArmor(int nSide) const override;
  const int GetRandomArmor(int nSide) const override;

  bool IsLockingTiles() const override;
  // Is it possible to rotate to the vNewDir direction if smallRect is taken as the bounding box
  bool CanRotateTo(SRect smallRect, const CVec2 &vNewDir, bool bWithUnits, bool bCanGoBackward = true) const override;
  bool CheckToTurn(WORD wNewDir) override;
  bool HasSuspendedPoint() const override;

  // for bored condition
  void UnRegisterAsBored(enum EUnitAckType eBoredType) override;
  void RegisterAsBored(enum EUnitAckType eBoredType) override;

  virtual class CUnitGuns *GetGuns() = 0;
  virtual const class CUnitGuns *GetGuns() const = 0;
  EUnitAckType GetGunsRejectReason() const override;
  bool DoesExistRejectGunsReason(const EUnitAckType &ackType) const;

  // goal resolution
  // unit kill rate with pStats from pGun
  const float GetKillSpeed(const SHPObjectRPGStats *pStats, const CVec2 &vCenter, CBasicGun *pGun) const;
  const float GetKillSpeed(const SHPObjectRPGStats *pStats, const CVec2 &vCenter, DWORD dwGuns) const;
  // speed of killing a unit from the best gun
  const float GetKillSpeed(class CAIUnit *pEnemy) const override;
  // unit kill speed from pGun
  virtual const float GetKillSpeed(class CAIUnit *pEnemy, class CBasicGun *pGun) const;
  // speed of killing a unit from the Gun set, numbers are specified by a mask
  virtual const float GetKillSpeed(CAIUnit *pEnemy, DWORD dwGuns) const;
  void UpdateTakenDamagePower(float fUpdate);
  const float GetTakenDamagePower() const { return fTakenDamagePower; }

  // reset time for scan periods
  void ResetTargetScan() override;
  // scan if it's time; 
  // returns: in the least significant bit - whether the target was found, in the second bit - whether a scan was performed
  BYTE AnalyzeTargetScan(CAIUnit *pCurTarget, bool bDamageUpdated, bool bScanForObstacles, IRefCount *pCheckBuilding = nullptr) override;
  // search for target, current target to attack - pCurTarget
  void LookForTarget(CAIUnit *pCurTarget, bool bDamageUpdated, CAIUnit **pBestTarget, class CBasicGun **pGun) override;
  // look for a target in the distance for artillery shelling, the current target for attack is pCurTarget
  virtual void LookForFarTarget(CAIUnit *pCurTarget, bool bDamageUpdated, CAIUnit **pBestTarget, class CBasicGun **pGun);
  // in order to look for enemies not only in the attack sector, but in the entire circle
  void SetCircularAttack(bool bCanAttack);
  // look for an obstacle.
  interface IObstacle *LookForObstacle() override;

  void UpdateNAttackingGrenages(const int nUpdate)
  {
    nGrenades += nUpdate;
    NI_ASSERT_T(nGrenades >= 0, "Wrong number of grenades");
  }

  const int GetNAttackingGrenages() const { return nGrenades; }

  // set to curTime the time for gun selection periods
  virtual void ResetGunChoosing();
  // if it's time to reselect gun, then reselect
  CBasicGun *AnalyzeGunChoose(CAIUnit *pEnemy);

  void EnemyKilled(CAIUnit *pEnemy);

  bool CanMoveForGuard() const override { return CanMove() && !GetStats()->IsTrain(); }
  // time after which the general will forget about the invisible unit
  virtual const float GetTimeToForget() const;
  CAIUnitInfoForGeneral *GetUnitInfoForGeneral() const;
  void SetLastVisibleTime(NTimer::STime time);

  // radius in which targets are scanned
  const float GetTargetScanRadius() override;
  // the unit is in a free search for targets
  virtual bool IsFreeEnemySearch() const { return bFreeEnemySearch; }

  // number of segments that have passed since the last SecondSegment call
  virtual const float GetPathSegmentsPeriod() const { return 1.0f; }
  virtual const NTimer::STime GetNextSecondPathSegmTime() const;

  float GetPriceMax() const override;
  // for Saving/Loading of static members
  friend class CStaticMembers;

  const NTimer::STime &GetBehUpdateDuration() const override { return SConsts::BEH_UPDATE_DURATION; }

  // killed: this unit + all units inside
  virtual void SendNTotalKilledUnits(int nPlayerOfShoot);

  const SVector GetLastKnownGoodTile() const override;

  const SAIExpLevel::SLevel &GetExpLevel() const;

  void UnitCommand(CAICommand *pCommand, bool bPlaceInQueue, bool bOnlyThisUnitCommand) override;

  // to send the general information about visible enemies, as well as send an update about the visibility of the unit
  void CalcVisibility();
  // the client is addicted!
  const bool IsVisibleByPlayer() override;

  // lock unit (if it has already been locked, then the old lock disappears)
  void Lock(const CBasicGun *pGun) override;
  // unlock unit (if locked by another gun, then nothing is done)
  void Unlock(const CBasicGun *pGun) override;
  // is it locked by any gun that is not equal to pGun
  bool IsLocked(const CBasicGun *pGun) const override;

  // changing the current type of cartridges.
  void SetActiveShellType(enum SWeaponRPGStats::SShell::EDamageType eShellType);

  // for planes
  void InitAviationPath();

  virtual void InstallAction(const EActionNotify eInstallAction, bool bAlreadyDone = false) {}
  virtual bool IsUninstalled() const { return true; }
  virtual bool IsInstalled() const { return true; }

  void AnimationSet(int nAnimation) override;
  void AnimationSegment() override;
  void Moved();
  void Stopped();
  void StopCurAnimation();

  virtual class CArtillery *GetArtilleryIfCrew() const { return nullptr; }
  virtual void TrackDamagedState(bool bTrackDamaged);

  void WantedToReveal(CAIUnit *pWhoRevealed);
  bool IsRevealed() const;

  bool IsInfantry() const override;
  // fired at by pUnit
  virtual void Grazed(CAIUnit *pUnit) {}

  void NullCreationTime() { creationTime = 0; }

  const int GetNVisIndexInUnits() const { return nVisIndexInUnits; }
  void SetNVisIndexInUnits(const int _nVisIndexInUnits) { nVisIndexInUnits = _nVisIndexInUnits; }

  bool CanMoveAfterUserCommand() const override;
};

#endif // __AI_UNIT_H__