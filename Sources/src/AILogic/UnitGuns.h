#ifndef __UNIT_GUNS_H__
#define __UNIT_GUNS_H__
// //////////////////////////////////////////////////////////// 
#pragma once
// //////////////////////////////////////////////////////////// 
#include "Guns.h"

interface IStaticPath;

// **********************************************************************
// *All unit weapons*
// **********************************************************************

class CUnitGuns : public IRefCount
{
  DECLARE_SERIALIZE;

  struct SWeaponPathInfo
  {
    float fRadius;
    NTimer::STime time;
    CPtr<IStaticPath> pStaticPath;
  };

  float fMaxFireRange;
  bool bCanShootToPlanes;

  std::vector<CPtr<SCommonGunInfo>> commonGunsInfo;
  std::vector<CObj<CBasicGun>> guns;
  std::vector<int> gunsBegins;
  int nCommonGuns;
  // with priority 0
  int nMainGun;

  //
  void FindTimeToTurn(class CAIUnit *pOwner, WORD wWillPower, class CTurret *pTurret, class CAIUnit *pEnemy, const SVector &finishTile, bool bIsEnemyInFireRange, NTimer::STime *pTimeToTurn) const;
  bool FindTimeToGo(class CAIUnit *pUnit, class CAIUnit *pEnemy, std::list<SWeaponPathInfo> *pPathInfo, const SWeaponRPGStats *pStats, SWeaponPathInfo *pInfo) const;

  bool FindTimeToStatObjGo(class CAIUnit *pUnit, class CStaticObject *pObj, std::list<SWeaponPathInfo> *pPathInfo, const SWeaponRPGStats *pStats, SWeaponPathInfo *pInfo) const;

public:
  CUnitGuns() : fMaxFireRange(-1), bCanShootToPlanes(false), nCommonGuns(0), nMainGun(0) {}
  virtual void Init(class CCommonUnit *pCommonUnit) = 0;

  void AddGun(const interface IGunsFactory &gunsFactory, const SWeaponRPGStats *pWeapon, int *nGuns, int nAmmo);
  void SetOwner(class CAIUnit *pUnit);

  const BYTE GetNTotalGuns() const { return guns.size(); }
  void Segment();

  //
  virtual int GetNGuns() const { return guns.size(); }
  virtual class CBasicGun *GetGun(const int n) const { return guns[n]; }
  // if there are guns that can be aimed at, then it displays the first one, otherwise 0
  virtual class CBasicGun *GetFirstArtilleryGun() const = 0;

  class CBasicGun *ChooseGunForStatObj(class CAIUnit *pOwner, class CStaticObject *pObj, NTimer::STime *pTime);

  const bool CanShootToPlanes() const { return bCanShootToPlanes; }
  float GetMaxFireRange(const class CAIUnit *pOwner) const;

  const int GetNCommonGuns() const { return nCommonGuns; }
  const SBaseGunRPGStats &GetCommonGunStats(int nCommonGun) const;
  int GetNAmmo(int nCommonGun) const;
  // nAmmo with sign
  void ChangeAmmo(int nCommonGun, int nAmmo);
  bool IsCommonGunFiring(const int nCommonGun) const { return commonGunsInfo[nCommonGun]->bFiring; }

  // gives reject reason to the highest priority gun among those who refused to shoot
  const EUnitAckType GetRejectReason() const;
  bool DoesExistRejectReason(const EUnitAckType &ackType) const;

  // gun with priority 0
  class CBasicGun *GetMainGun() const;

  virtual const int GetActiveShellType() const = 0;
  virtual bool SetActiveShellType(enum SWeaponRPGStats::SShell::EDamageType eShellType) = 0;
};

class CMechUnitGuns : public CUnitGuns
{
  OBJECT_COMPLETE_METHODS(CMechUnitGuns);
  DECLARE_SERIALIZE;

  int nFirstArtGun;

public:
  CMechUnitGuns() : nFirstArtGun(-1) {}
  void Init(class CCommonUnit *pCommonUnit) override;

  bool SetActiveShellType(enum SWeaponRPGStats::SShell::EDamageType eShellType) override;
  const int GetActiveShellType() const override;

  class CBasicGun *GetFirstArtilleryGun() const override;
};

class CInfantryGuns : public CUnitGuns
{
  OBJECT_COMPLETE_METHODS(CInfantryGuns);
  DECLARE_SERIALIZE;

public:
  void Init(class CCommonUnit *pCommonUnit) override;

  class CBasicGun *GetFirstArtilleryGun() const override { return nullptr; }

  bool SetActiveShellType(const enum SWeaponRPGStats::SShell::EDamageType eShellType) override { return false; }
  const int GetActiveShellType() const override { return SWeaponRPGStats::SShell::DAMAGE_HEALTH; }
};

#endif // __UNIT_GUNS_H__