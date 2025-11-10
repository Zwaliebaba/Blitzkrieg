#ifndef __TECHNICS_H__
#define __TECHNICS_H__

#pragma once

#include "AIUnit.h"

class CExistingObject;
class CTurret;
class CUnitGuns;
class CSoldier;
class CFormation;
class CArtillery;
class CEntrenchmentTankPit;

// just a military machine, base class
class CMilitaryCar : public CAIUnit
{
  DECLARE_SERIALIZE;

  CGDBPtr<SMechUnitRPGStats> pStats;

  // gun barrels
  CPtr<CUnitGuns> pGuns;

  // rotating cannon
  std::vector<CObj<CTurret>> turrets;

  // passengers
  std::list<CPtr<CSoldier>> pass;

  CPtr<CFormation> pLockingUnit;
  float fDispersionBonus;
  NTimer::STime timeLastHeal;// last time of treatment
  NTimer::STime lastResupplyMorale;// last morale addition time

  //
  // passenger coordinates n
  const CVec2 GetPassengerCoordinates(int n);

protected:
  void InitGuns() override;
  const class CUnitGuns *GetGuns() const override { return pGuns; }
  class CUnitGuns *GetGuns() override { return pGuns; }
  void PrepareToDelete() override;

public:
  // we redefine this function in subclasses
  void Init(const CVec2 &center, int z, const SUnitBaseRPGStats *pStats, float fHP, WORD dir, BYTE player, WORD id, EObjVisType eVisType, int dbID) override;

  void Lock(class CFormation *_pLockingUnit);
  bool IsLocked() const { return pLockingUnit != nullptr; }
  void Unlock() { pLockingUnit = nullptr; }

  const SUnitBaseRPGStats *GetStats() const override { return pStats; }
  IStatesFactory *GetStatesFactory() const override =0;

  // distance from the center to the point from where you can directly run to the entrance point
  virtual float GetDistanceToLandPoint() const;

  virtual BYTE GetNAvailableSeats() const { return pStats->nPassangers - pass.size(); }
  virtual BYTE GetNPassengers() const { return pass.size(); }
  virtual void AddPassenger(class CSoldier *pUnit);
  virtual class CSoldier *GetPassenger(int n);

  const CVec2 GetEntrancePoint() const;

  // remove all passengers
  virtual void ClearAllPassengers();
  virtual void DelPassenger(int n);
  virtual void DelPassenger(class CSoldier *pSoldier);

  void Segment() override;

  CTurret *GetTurret(const int nTurret) const override { return turrets[nTurret]; }
  const int GetNTurrets() const override { return turrets.size(); }

  void GetShotInfo(struct SAINotifyMechShot *pShotInfo) const override
  {
    pShotInfo->typeID = GetShootAction();
    pShotInfo->pObj = const_cast<CMilitaryCar *>(this);
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

  class CBasicGun *GetFirstArtilleryGun() const override;

  class CBasicGun *ChooseGunForStatObj(class CStaticObject *pObj, NTimer::STime *pTime) override;
  float GetMaxFireRange() const override;

  bool IsMech() const override { return true; }

  // bonuses
  const float GetDispersionBonus() const override;
  const void SetDispersionBonus(const float fBonus) override { fDispersionBonus = fBonus; }

  void GetRangeArea(struct SShootAreas *pRangeArea) const override;
  virtual class CArtillery *GetTowedArtillery() const = 0;

  const int CanGoBackward() const override { return GetTowedArtillery() == nullptr; }

  const CVec2 GetHookPoint() const;
  const CVec3 GetHookPoint3D() const;

  // killed: this unit + all units inside
  void SendNTotalKilledUnits(int nPlayerOfShoot) override;
  void LookForTarget(CAIUnit *pCurTarget, bool bDamageUpdated, CAIUnit **pBestTarget, class CBasicGun **pGun) override;

  virtual bool HasTowedArtilleryCrew() const { return false; }
  virtual void SetTowedArtilleryCrew(class CFormation *pFormation) {}
  virtual CFormation *GetTowedArtilleryCrew() { return nullptr; }
};

// can sit in the TankPit. 
// you must first execute the command to exit TankPit.
class CTank : public CMilitaryCar
{
  OBJECT_COMPLETE_METHODS(CTank);
  DECLARE_SERIALIZE;

  bool bTrackDamaged;// true if the tank's track is broken

  WORD wDangerousDir;
  bool bDangerousDirSet;
  bool bDangerousDirSetInertia;
  NTimer::STime nextTimeOfDangerousDirScan;
  NTimer::STime lastTimeOfDangerousDirChanged;

  WORD wDangerousDirUnderFire;
  float fDangerousDamageUnderFire;

  //
  void ScanForDangerousDir();

public:
  void Init(const CVec2 &center, int z, const SUnitBaseRPGStats *pStats, float fHP, WORD dir, BYTE player, WORD id, EObjVisType eVisType, int dbID) override;
  IStatesFactory *GetStatesFactory() const override;

  bool IsTrackDamaged() const { return bTrackDamaged; }
  void RepairTrack();// fixed the caterpillar

  void TakeDamage(float fDamage, const SWeaponRPGStats::SShell *pShell, int nPlayerOfShoot, CAIUnit *pShotUnit) override;
  bool CanMove() const override;
  bool CanMovePathfinding() const override;
  bool CanRotate() const override;
  bool CanTurnToFrontDir(WORD wDir) override;

  const bool NeedDeinstall() const override { return bTrackDamaged; }

  class CArtillery *GetTowedArtillery() const override { return nullptr; }

  const NTimer::STime &GetBehUpdateDuration() const override
  {
    if (RPG_TYPE_SPG_AAGUN == GetStats()->type) return SConsts::AA_BEH_UPDATE_DURATION;
    return SConsts::BEH_UPDATE_DURATION;
  }

  void Segment() override;

  bool IsDangerousDirExist() const override { return bDangerousDirSetInertia; }
  const WORD GetDangerousDir() const override { return wDangerousDir; }
  void Grazed(CAIUnit *pUnit) override;

  bool CanMoveAfterUserCommand() const override;
};

// transport, transports marine resources,
// can hook guns
class CAITransportUnit : public CMilitaryCar
{
  OBJECT_COMPLETE_METHODS(CAITransportUnit);
  DECLARE_SERIALIZE;

  float fResursUnits;// number of RUs the truck has
  CPtr<CArtillery> pTowedArtillery;
  CPtr<CAIUnit> pMustTow;// artillery, that this truck must tow (for general intendant)

  using CExternLoaders = std::list<CPtr<CFormation>>;
  CExternLoaders externLoaders;// porters waiting for transport
  CPtr<CFormation> pTowedArtilleryCrew;// when artillery is attached the crew.

  // for group reinforcement of artillery,
  // selects the unit from our group closest to the artillery and returns its nUniqueId
  const int GetNUnitToTakeArtillery(bool bPlaceInQueue, CAIUnit *pUnitToTake);

public:
  void Init(const CVec2 &center, int z, const SUnitBaseRPGStats *pStats, float fHP, WORD dir, BYTE player, WORD id, EObjVisType eVisType, int dbID) override;

  // for the repair process. 
  float GetResursUnitsLeft() const { return fResursUnits; }
  void SetResursUnitsLeft(float _fResursUnits);
  void DecResursUnitsLeft(float dRU);

  // towing
  bool IsTowing() const override;
  class CArtillery *GetTowedArtillery() const override { return pTowedArtillery; }
  void SetTowedArtillery(class CArtillery *pTowedArtillery);

  IStatesFactory *GetStatesFactory() const override;
  bool CanCommandBeExecuted(class CAICommand *pCommand) override;

  void GetRPGStats(struct SAINotifyRPGStats *pStats) override;
  const int GetUnitState() const override;

  static void PrepareLoaders(CFormation *pLoaderSquad, CAITransportUnit *pTransport);
  static void FreeLoaders(CFormation *pLoaderSquad, CAITransportUnit *pTransport);
  void Segment() override;

  void AddExternLoaders(CFormation *pLoaders);
  void Die(bool fromExplosion, float fDamage) override;

  // towed artillery crew management
  bool HasTowedArtilleryCrew() const override;
  void SetTowedArtilleryCrew(class CFormation *pFormation) override;
  CFormation *GetTowedArtilleryCrew() override;

  // 
  void SetMustTow(class CAIUnit *_pUnit);
  bool IsMustTow() const;

  void UnitCommand(CAICommand *pCommand, bool bPlaceInQueue, bool bOnlyThisUnitCommand) override;

  bool CanHookUnit(class CAIUnit *pUnitToHook) const override;
};

#endif // __TECHNICS_H__