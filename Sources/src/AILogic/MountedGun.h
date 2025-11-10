#ifndef __MOUNTED_GUN_H__
#define __MOUNTED_GUN_H__

#pragma once
// //////////////////////////////////////////////////////////// 
#include "../Common/Actions.h"
#include "Guns.h"
// //////////////////////////////////////////////////////////// 
class CMountedTurret;

// //////////////////////////////////////////////////////////// 
class CCommonMountedGun
{
  DECLARE_SERIALIZE;

  CGDBPtr<SBuildingRPGStats> pBuildingStats;
  class CBuilding *pBuilding;
  CPtr<CMountedTurret> pTurret;
  int nSlot;

public:
  CCommonMountedGun() {}
  CCommonMountedGun(class CBuilding *pObject, class CMountedTurret *pTurret, int nSlot);

  virtual const SBaseGunRPGStats &GetGun() const;
  virtual const SWeaponRPGStats *GetWeapon() const;
  virtual bool IsOnTurret() const { return true; }

  virtual class CTurret *GetTurret() const;

  virtual void GetInfantryShotInfo(SAINotifyInfantryShot *pInfantryShotInfo, const NTimer::STime &time) const;
};

// //////////////////////////////////////////////////////////// 
template<class T>
class CMountedGun : public T, public CCommonMountedGun
{
  OBJECT_COMPLETE_METHODS(CMountedGun<T>);
  DECLARE_SERIALIZE;

public:
  CMountedGun() {}

  CMountedGun(class CBuilding *pObject, class CMountedTurret *pTurret, const int nSlot, const BYTE nShellType, SCommonGunInfo *pCommonGunInfo, IGunsFactory::EGunTypes eType)
    : CCommonMountedGun(pObject, pTurret, nSlot), T(0, nShellType, pCommonGunInfo, eType) {}

  const SBaseGunRPGStats &GetGun() const override { return CCommonMountedGun::GetGun(); }
  const SWeaponRPGStats *GetWeapon() const override { return CCommonMountedGun::GetWeapon(); }
  bool IsOnTurret() const override { return CCommonMountedGun::IsOnTurret(); }

  class CTurret *GetTurret() const override { return CCommonMountedGun::GetTurret(); }

  void GetInfantryShotInfo(SAINotifyInfantryShot *pInfantryShotInfo, const NTimer::STime &time) const override;
};

// //////////////////////////////////////////////////////////// 
template<class T>
void CMountedGun<T>::GetInfantryShotInfo(SAINotifyInfantryShot *pInfantryShotInfo, const NTimer::STime &time) const
{
  CCommonMountedGun::GetInfantryShotInfo(pInfantryShotInfo, time);
  pInfantryShotInfo->cShell = T::nShellType;
}

// //////////////////////////////////////////////////////////// 
template<class T>
int CMountedGun<T>::operator&(IStructureSaver &ss)
{
  CSaverAccessor saver = &ss;

  saver.AddTypedSuper(1, static_cast<T *>(this));
  saver.AddTypedSuper(2, static_cast<CCommonMountedGun *>(this));

  return 0;
}

// //////////////////////////////////////////////////////////// 
class CMountedGunsFactory : public IGunsFactory
{
  class CBuilding *pBuilding;
  class CMountedTurret *pMountedTurret;
  const int nCommonGun;

public:
  CMountedGunsFactory(class CBuilding *_pBuilding, class CMountedTurret *_pMountedTurret, const int _nCommonGun)
    : pBuilding(_pBuilding), pMountedTurret(_pMountedTurret), nCommonGun(_nCommonGun) {}

  int GetNCommonGun() const override { return nCommonGun; }

  class CBasicGun *CreateGun(EGunTypes eType, int nShell, SCommonGunInfo *pCommonGunInfo) const override;
};

// //////////////////////////////////////////////////////////// 
#endif // __MOUNTED_GUN_H__