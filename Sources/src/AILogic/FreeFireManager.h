#ifndef __FREE_FIRE_MANAGER_H__
#define __FREE_FIRE_MANAGER_H__

#pragma once
// //////////////////////////////////////////////////////////// 
class CAIUnit;
// //////////////////////////////////////////////////////////// 
// to attack with secondary cannons.
// if the turrets on which the guns are located are not locked, then rotates the turrets.
class CFreeFireManager
{
  DECLARE_SERIALIZE;

  enum { TIME_TO_CHECK = 1500 };

  struct SShotInfo
  {
    DECLARE_SERIALIZE;

  public:
    CPtr<CAIUnit> pTarget;
    CVec2 shootingPos;
    WORD unitDir;
    WORD gunDir;

    SShotInfo() : shootingPos(VNULL2), unitDir(0), gunDir(0) {}

    bool NeedAim(CAIUnit *pNewTarget, class CBasicGun *pGun) const;
    void SetInfo(CAIUnit *pNewTarget, class CBasicGun *pGun);
  };

  std::vector<SShotInfo> shootInfo;

  NTimer::STime lastCheck;

public:
  CFreeFireManager() : lastCheck(0) {}
  CFreeFireManager(class CCommonUnit *pOwner);
  // it is considered that base is already locked by pActiveGun
  void Analyze(class CCommonUnit *pUnit, class CBasicGun *pActiveGun);
};

// //////////////////////////////////////////////////////////// 
#endif // __FREE_FIRE_MANAGER_H__