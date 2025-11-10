#ifndef __SHELL_H__
#define __SHELL_H__

#pragma once
// ///////////////////////////////////////////////////////////// 
#include <queue>
#include "UpdatableObject.h"
#include "LinkObject.h"
#include <zlib.h>
// ///////////////////////////////////////////////////////////// 
class CAIUnit;
// ///////////////////////////////////////////////////////////// 
// **********************************************************************
// *Hits*
// **********************************************************************
// ///////////////////////////////////////////////////////////// 
class CHitInfo : public CLinkObject
{
  OBJECT_COMPLETE_METHODS(CHitInfo);
  DECLARE_SERIALIZE;

public:
  CGDBPtr<SWeaponRPGStats> pWeapon;
  WORD wShell;
  WORD wDir;

  CPtr<IRefCount> pVictim;// to hit a unit
  CVec3 explCoord;// to hit the ground

  SAINotifyHitInfo::EHitType eHitType;

  CHitInfo() {}

  CHitInfo(const SWeaponRPGStats *_pWeapon, const WORD _wShell, const WORD &_wDir, IRefCount *_pVictim, const SAINotifyHitInfo::EHitType _eHitType, const CVec3 &_explCoord)
    : pWeapon(_pWeapon), wShell(_wShell), wDir(_wDir), pVictim(_pVictim), explCoord(_explCoord), eHitType(_eHitType) { SetUniqueId(); }

  CHitInfo(const SWeaponRPGStats *_pWeapon, const WORD _wShell, const WORD &_wDir, const CVec3 &_explCoord, const SAINotifyHitInfo::EHitType _eHitType)
    : pWeapon(_pWeapon), wShell(_wShell), wDir(_wDir), pVictim(nullptr), explCoord(_explCoord), eHitType(_eHitType) { SetUniqueId(); }

  CHitInfo(const class CExplosion *pExpl, IRefCount *pVictim, const enum SAINotifyHitInfo::EHitType &eHitType, const CVec3 &explCoord);

  void GetHitInfo(struct SAINotifyHitInfo *pHitInfo) const override;

  const bool IsVisible(const BYTE party) const override { return true; }
  void GetTilesForVisibility(CTilesSet *pTiles) const override { pTiles->clear(); }
  bool ShouldSuspendAction(const EActionNotify &eAction) const override { return false; }
};

// ///////////////////////////////////////////////////////////// 
// **********************************************************************
// * “trajectories *
// **********************************************************************
// ///////////////////////////////////////////////////////////// 
interface IBallisticTraj : IRefCount
{
  virtual const NTimer::STime &GetExplTime() const = 0;
  virtual const NTimer::STime &GetStartTime() const = 0;

  virtual const CVec3 &GetStartPoint() const = 0;
  virtual const WORD GetStart2DDir() const = 0;

  virtual const CVec3 GetCoordinates() const = 0;

  virtual const SWeaponRPGStats::SShell::ETrajectoryType GetTrajType() const = 0;
};

// ///////////////////////////////////////////////////////////// 
class CBallisticTraj : public IBallisticTraj
{
  OBJECT_COMPLETE_METHODS(CBallisticTraj);
  DECLARE_SERIALIZE;

  CVec3 vStart3D;
  // speed
  float fVx, fVy;
  // acceleration of free fall

  WORD wAngle;// vertical angle

  WORD wDir;
  CVec2 vDir;

  float fG;// for a given trajectory, the acceleration of free fall is

  NTimer::STime startTime, explTime;

  SWeaponRPGStats::SShell::ETrajectoryType eType;

public:
  CBallisticTraj() {}
  CBallisticTraj(const CVec3 &vStart, const CVec2 &vFinish, float fV, SWeaponRPGStats::SShell::ETrajectoryType eType, WORD wMaxAngle, float fMaxRange);

  const NTimer::STime &GetExplTime() const override { return explTime; }
  const NTimer::STime &GetStartTime() const override { return startTime; }
  const CVec3 &GetStartPoint() const override { return vStart3D; }
  const WORD GetStart2DDir() const override { return wDir; }

  const CVec3 GetCoordinates() const override;

  static WORD GetTrajectoryZAngle(const CVec2 &vToAim, float z, float fV, SWeaponRPGStats::SShell::ETrajectoryType eType, WORD wMaxAngle, float fMaxRange);

  const SWeaponRPGStats::SShell::ETrajectoryType GetTrajType() const override { return eType; }
};

// ///////////////////////////////////////////////////////////// 
class CFakeBallisticTraj : public IBallisticTraj
{
  OBJECT_COMPLETE_METHODS(CFakeBallisticTraj);
  DECLARE_SERIALIZE;

  NTimer::STime startTime, explTime;

  CVec3 point;
  CVec3 v;
  float A1, A2;
  WORD wDir;

public:
  CFakeBallisticTraj() {}
  CFakeBallisticTraj(const CVec3 &point, const CVec3 &v, const NTimer::STime &explTime, float A1, float A2);

  const NTimer::STime &GetExplTime() const override { return explTime; }
  const NTimer::STime &GetStartTime() const override { return startTime; }

  const CVec3 &GetStartPoint() const override { return point; }
  const WORD GetStart2DDir() const override { return wDir; }

  const CVec3 GetCoordinates() const override;
  static WORD GetTrajectoryZAngle(const CVec2 &vToAim, const float z, float fV, const SWeaponRPGStats::SShell::ETrajectoryType eType) { return 0; }

  const SWeaponRPGStats::SShell::ETrajectoryType GetTrajType() const override { return SWeaponRPGStats::SShell::TRAJECTORY_CANNON; }
};

// ///////////////////////////////////////////////////////////// 
class CBombBallisticTraj : public IBallisticTraj
{
  OBJECT_COMPLETE_METHODS(CBombBallisticTraj);

  DECLARE_SERIALIZE;

  CVec3 point;
  CVec3 v;
  CVec2 vRandAcc;
  WORD wDir;
  NTimer::STime startTime, explTime;

public:
  CBombBallisticTraj() {};
  CBombBallisticTraj(const CVec3 &point, const CVec3 &v, const NTimer::STime &explTime, const CVec2 &vRandAcc);

  const NTimer::STime &GetExplTime() const override { return explTime; }
  const NTimer::STime &GetStartTime() const override { return startTime; }

  const CVec3 &GetStartPoint() const override { return point; }
  const WORD GetStart2DDir() const override { return wDir; }

  const CVec3 GetCoordinates() const override;

  const SWeaponRPGStats::SShell::ETrajectoryType GetTrajType() const override { return SWeaponRPGStats::SShell::TRAJECTORY_BOMB; }

  static float GetCoeff(const float &timeDiff);
  static WORD GetTrajectoryZAngle(const CVec2 &vToAim, const float z, float fV, const SWeaponRPGStats::SShell::ETrajectoryType eType) { return 16384 * 3; }
  static CVec3 CalcTrajectoryFinish(const CVec3 &vSourcePoint, const CVec3 &vInitialSpeed, const CVec2 &vRandAcc);
  static float GetTimeOfFly(float fZ, float fZSpeed);
};

// **********************************************************************
// * ¬explosions *
// **********************************************************************
// ///////////////////////////////////////////////////////////// 
class CExplosion : public IRefCount
{
  DECLARE_SERIALIZE;

protected:
  BYTE nShellType;
  CGDBPtr<SWeaponRPGStats> pWeapon;
  CPtr<CAIUnit> pUnit;

  CVec2 explCoord;
  float z;
  WORD attackDir;
  int nPlayerOfShoot;

  CPtr<CHitInfo> pHitToSend;

  //
  const SAINotifyHitInfo::EHitType ProcessExactHit(class CAIUnit *pTarget, const SRect &combatRect, const CVec2 &explCoord, int nRandPiercing, int nRandArmor) const;
  void Init(class CAIUnit *pUnit, const struct SWeaponRPGStats *pWeapon, float fDispersion, float fDispRatio, const CVec2 &_explCoord, float _z, const CVec2 &attackerPos, BYTE nShellType, bool bRandomize, int nPlayerOfShoot);

public:
  CExplosion() : nPlayerOfShoot(-1) {}
  CExplosion(CAIUnit *pUnit, const class CBasicGun *pGun, const CVec2 &explCoord, float z, const CVec2 &attackerPos, BYTE nShellType, bool bRandomize = true);
  CExplosion(CAIUnit *pUnit, const struct SWeaponRPGStats *pWeapon, const CVec2 &explCoord, float z, const CVec2 &attackerPos, BYTE nShellType, bool bRandomize = true);

  const CVec2 &GetExplCoordinates() const { return explCoord; }
  const float &GetExplZ() const { return z; }
  CAIUnit *GetWhoFire() const { return pUnit; }

  const SWeaponRPGStats *GetWeapon() const { return pWeapon; }
  const BYTE GetShellType() const { return nShellType; }
  const SWeaponRPGStats::SShell GetShellStats() const { return pWeapon->shells[nShellType]; }
  const WORD GetAttackDir() const { return attackDir; }

  const int GetRandomPiercing() const;
  const float GetRandomDamage() const;

  const int GetPartyOfShoot() const;
  const int GetPlayerOfShoot() const;

  virtual void Explode() = 0;
  float GetMaxDamage() const;
  SWeaponRPGStats::SShell::ETrajectoryType GetTrajectoryType() const { return pWeapon->shells[nShellType].trajectory; }

  // if the explosion is moral, then it will return true.
  bool ProcessMoraleExplosion() const;
  // if the explosion is smoke, then it will return true.
  bool ProcessSmokeScreenExplosion() const;

  void AddHitToSend(CHitInfo *pHit);
};

// ///////////////////////////////////////////////////////////// 
class CBurstExpl : public CExplosion
{
  OBJECT_COMPLETE_METHODS(CBurstExpl);
  DECLARE_SERIALIZE;
  int nArmorDir;

public:
  CBurstExpl() {}
  // nArmorDir == 0 - just in a flat direction (this is for projectiles)
  // nArmorDir == 1 - explosion under the bottom (dl¤ min)
  // nArmorDir == 2 - explosion above the roof

  CBurstExpl(CAIUnit *pUnit, const class CBasicGun *pGun, const CVec2 &explCoord, float z, const CVec2 &attackerPos, BYTE nShellType, bool bRandomize = true, int ArmorDir = 0);
  CBurstExpl(CAIUnit *pUnit, const SWeaponRPGStats *pWeapon, const CVec2 &explCoord, float z, const CVec2 &attackerPos, BYTE nShellType, bool bRandomize = true, int ArmorDir = 0);

  void Explode() override;
};

// ///////////////////////////////////////////////////////////// 
class CCumulativeExpl : public CExplosion
{
  OBJECT_COMPLETE_METHODS(CCumulativeExpl);
  DECLARE_SERIALIZE;
  int nArmorDir;

public:
  CCumulativeExpl() {}
  CCumulativeExpl(CAIUnit *pUnit, const class CBasicGun *pGun, const CVec2 &explCoord, float z, const CVec2 &attackerPos, BYTE nShellType, bool bRandomize = true);

  void Explode() override;
};

// ///////////////////////////////////////////////////////////// 
// **********************************************************************
// * - people *
// **********************************************************************
// ///////////////////////////////////////////////////////////// 
// projectile hitting instantly
class CMomentShell
{
  DECLARE_SERIALIZE;

  CPtr<CExplosion> expl;

public:
  CMomentShell(CExplosion *_expl) : expl(_expl) {}

  const CVec2 &GetExplCoordinates() const { return expl->GetExplCoordinates(); }
  const float GetExplZ() const { return expl->GetExplZ(); }
  void Explode() { expl->Explode(); }

  float GetMaxDamage() const { return expl->GetMaxDamage(); }
};

// ///////////////////////////////////////////////////////////// 
// projectile
class CShell
{
  DECLARE_SERIALIZE;

  NTimer::STime explTime;
  CPtr<CExplosion> expl;
  int nGun;

  float vStartVisZ, vFinishVisZ;

protected:
  const float GetStartVisZ() const { return vStartVisZ; }
  const float GetFinishVisZ() const { return vFinishVisZ; }

public:
  CShell() {}
  CShell(const NTimer::STime &explTime, CExplosion *expl, int nGun);

  const NTimer::STime GetExplTime() const { return explTime; }

  const CVec2 &GetExplCoordinates() const { return expl->GetExplCoordinates(); }
  const float GetExplZ() const { return expl->GetExplZ(); }
  const SWeaponRPGStats *GetWeapon() const { return expl->GetWeapon(); }
  const BYTE GetShellType() const { return expl->GetShellType(); }

  IRefCount *GetWhoFired() const;
  const int GetNGun() const { return nGun; }

  void Explode() { expl->Explode(); }
  float GetMaxDamage() const { return expl->GetMaxDamage(); }
  SWeaponRPGStats::SShell::ETrajectoryType GetTrajectoryType() const { return expl->GetTrajectoryType(); }
};

// ///////////////////////////////////////////////////////////// 
// invisible projectile
class CInvisShell : public IRefCount, public CShell
{
  OBJECT_COMPLETE_METHODS(CInvisShell);
  DECLARE_SERIALIZE;

public:
  CInvisShell() {}

  CInvisShell(const NTimer::STime &explTime, CExplosion *expl, const int nGun)
    : CShell(explTime, expl, nGun) {}

  bool operator <(const CInvisShell &shell) { return GetExplTime() > shell.GetExplTime(); }
};

// ///////////////////////////////////////////////////////////// 
inline bool operator <(const CPtr<CInvisShell> &shell1, const CPtr<CInvisShell> &shell2) { return (*shell1) < (*shell2); }
// ///////////////////////////////////////////////////////////// 
// visible projectile
class CVisShell : public CLinkObject, public CShell
{
  OBJECT_COMPLETE_METHODS(CVisShell);

  DECLARE_SERIALIZE;
  CPtr<IBallisticTraj> pTraj;
  CVec3 center;
  CVec3 speed;
  bool bVisible;

  void CalcVisibility();

public:
  CVisShell() {}

  CVisShell(CExplosion *_expl, IBallisticTraj *_pTraj, const int nGun)
    : CShell(_pTraj->GetExplTime(), _expl, nGun), pTraj(_pTraj), center(_pTraj->GetStartPoint()), speed(VNULL3), bVisible(true) { SetUniqueId(); }

  const NTimer::STime GetStartTime() const { return pTraj->GetStartTime(); }

  void GetPlacement(struct SAINotifyPlacement *pPlacement, NTimer::STime timeDiff) override;
  void GetSpeed3(CVec3 *pSpeed) const override { *pSpeed = speed; }
  void GetProjectileInfo(struct SAINotifyNewProjectile *pProjectileInfo) override;

  void Segment();

  const CVec3 GetCoordinates() const { return pTraj->GetCoordinates(); }

  float GetTerrainHeight(float x, float y, NTimer::STime timeDiff) const override;
  const bool IsVisible(const BYTE party) const override { return true; }
  void GetTilesForVisibility(CTilesSet *pTiles) const override { pTiles->clear(); }
  bool ShouldSuspendAction(const EActionNotify &eAction) const override { return false; }

  virtual const bool IsVisibleByPlayer() const;
};

// ///////////////////////////////////////////////////////////// 
// **********************************************************************
// * - treasure of shells *
// **********************************************************************
// ///////////////////////////////////////////////////////////// 
class CShellsStore
{
  DECLARE_SERIALIZE;
  // all invisible projectiles
  std::priority_queue<CPtr<CInvisShell>> invisShells;
  // all visible projectiles
  std::list<CPtr<CVisShell>> visShells;

public:
  CShellsStore() {};
  void Clear();

  void AddShell(CMomentShell &shell);
  void AddShell(CInvisShell *pShell);
  void AddShell(CVisShell *pShell);

  void Segment();

  // for testing¤ multiplayer
  void UpdateCheckSum(uLong *pCheckSum);
};

// ///////////////////////////////////////////////////////////// 
#endif	// __SHELL_H__