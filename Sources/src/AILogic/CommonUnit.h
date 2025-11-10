#ifndef __COMMON_UNIT_H__
#define __COMMON_UNIT_H__

#pragma once

#include "LinkObject.h"
#include "BasePathUnit.h"
#include "GroupUnit.h"
#include "QueueUnit.h"

// the reasons why they are trying to remove camouflage from a unit,
enum ECamouflageRemoveReason
{
  ECRR_SELF_MOVE,
  ECRR_SELF_SHOOT,
  ECRR_GOOD_VISIBILITY,
  ECRR_USER_COMMAND,
};

class CBasicGun;

struct SBehaviour
{
  enum EMoving
  {
    EMRoaming = 0,
    EMFollow = 1,
    EMHoldPos = 2
  };

  enum EFire
  {
    EFAtWill = 0,
    EFReturn = 1,
    EFNoFire = 2
  };

  DECLARE_SERIALIZE;

public:
  EMoving moving;
  EFire fire;

  SBehaviour() : moving(EMRoaming), fire(EFAtWill) {}
  SBehaviour(const EMoving _moving, const EFire _fire) : moving(_moving), fire(_fire) {}
};

interface IShootEstimator;
interface IScenarioUnit;
class CAIUnit;

class CCommonUnit : public CLinkObject, public IBasePathUnit, public CGroupUnit, public CQueueUnit
{
  DECLARE_SERIALIZE;

  int dbID;

  CPtr<IScenarioUnit> pScenarioUnit;

  SBehaviour beh;
  NTimer::STime lastBehTime;

  CPtr<CBasicGun> pLockingGun;
  CVec2 vBattlePos;
  WORD wReserveDir;
  CPtr<CAIUnit> pTruck;

  bool bSelectable;

  float fDesirableSpeed;
  CPtr<CCommonUnit> pFollowedUnit;
  // minimum desired speed due to someone following us
  float fMinFollowingSpeed;
  CVec2 vFollowShift;

  CPtr<IShootEstimator> pShootEstimator;

  bool bCanBeFrozenByState;
  bool bCanBeFrozenByScan;
  NTimer::STime nextFreezeScan;

public:
  CCommonUnit() {}

  int GetMovingType() const override { return 0; }
  bool IsSelectable() const override { return bSelectable; }
  virtual void Init(int dbID);

  void SetScenarioUnit(interface IScenarioUnit *pScenarioUnit);
  IScenarioUnit *GetScenarioUnit() const;

  virtual int GetNGuns() const = 0;
  virtual CBasicGun *GetGun(int n) const = 0;

  virtual CBasicGun *ChooseGunForStatObj(class CStaticObject *pObj, NTimer::STime *pTime) = 0;
  virtual CBasicGun *ChooseGunForStatObjWOTime(class CStaticObject *pObj);

  bool CanShootToUnitWoMove(class CAIUnit *pTarget);

  virtual void SetSelectable(bool bSelectable);

  const BYTE GetPlayer() const override = 0;
  const BYTE GetParty() const;
  virtual void ChangePlayer(BYTE cPlayer) = 0;
  virtual void SetPlayerForEditor(int nPlayer) = 0;

  // what is the maximum damage that can be inflicted on a pTarget unit?
  virtual const float GetMaxDamage(class CCommonUnit *pTarget) const;

  virtual const float GetSightRadius() const = 0;

  bool CanGoToPoint(const CVec2 &point) const override;

  SBehaviour &GetBehaviour() { return beh; }
  const SBehaviour &GetBehaviour() const { return beh; }
  const NTimer::STime &GetLastBehTime() const { return lastBehTime; }
  NTimer::STime &GetLastBehTime() { return lastBehTime; }

  const bool IsVisible(BYTE party) const override = 0;

  const int GetDBID() const override { return dbID; }

  virtual const bool NeedDeinstall() const { return false; }
  // can it shoot now (for example, if it is not installed, then it cannot)
  virtual const bool CanShoot() const { return true; }
  virtual const bool CanShootToPlanes() const = 0;

  virtual void Fired(float fGunRadius, int nGun) = 0;

  virtual CBasicGun *GetFirstArtilleryGun() const { return nullptr; }

  virtual void SetAmbush() = 0;
  virtual void RemoveAmbush() = 0;

  virtual const NTimer::STime GetTimeToCamouflage() const = 0;
  virtual void SetCamoulfage() =0;
  virtual void RemoveCamouflage(ECamouflageRemoveReason eReason) =0;


  // about updating the range/shooting area
  virtual void UpdateArea(EActionNotify eAction) = 0;
  BYTE GetAIClass() const override { return AI_CLASS_ANY; }

  // lock unit (if it has already been locked, then the old lock disappears)
  virtual void Lock(const CBasicGun *pGun);
  // unlock unit (if locked by another gun, then nothing is done)
  virtual void Unlock(const CBasicGun *pGun);
  // is it locked by any gun that is not equal to pGun
  virtual bool IsLocked(const CBasicGun *pGun) const;

  virtual class CTurret *GetTurret(int nTurret) const = 0;
  virtual const int GetNTurrets() const = 0;
  virtual bool IsMech() const = 0;

  void SetBattlePos(const CVec2 &vPos, const WORD _wReserveDir = 0)
  {
    vBattlePos = vPos;
    wReserveDir = _wReserveDir;
  }

  bool DoesReservePosExist() const { return vBattlePos.x != -1.0f; }

  const CVec2 &GetBattlePos() const
  {
    NI_ASSERT_T(DoesReservePosExist(), "Reserve pos doesn't exist");
    return vBattlePos;
  }

  const WORD GetReserveDir() const
  {
    NI_ASSERT_T(DoesReservePosExist(), "Reserve pos doesn't exist");
    return wReserveDir;
  }

  void SetTruck(class CAIUnit *pUnit);
  class CAIUnit *GetTruck() const;

  // remove a unit from the map
  virtual void Disappear() = 0;
  // die
  virtual void Die(bool fromExplosion, float fDamage) = 0;

  // only the formation returns true
  virtual bool IsFormation() const { return false; }
  virtual bool IsInfantry() const { return false; }

  void SendAcknowledgement(EUnitAckType ack, bool bForce = false) override = 0;
  // ack for pCommand
  void SendAcknowledgement(CAICommand *pCommand, EUnitAckType ack, bool bForce = false) override = 0;

  const float GetMaxSpeedHere(const CVec2 &point, bool bAdjust = true) const override;
  void SetDesirableSpeed(float fDesirableSpeed) override;
  void UnsetDesirableSpeed() override;
  float GetDesirableSpeed() const override;
  void AdjustWithDesirableSpeed(float *pfMaxSpeed) const override;

  // follow state - we are following someone
  void SetFollowState(class CCommonUnit *pFollowedUnit);
  void UnsetFollowState();
  bool IsInFollowState();
  // return the unit we are following
  class CCommonUnit *GetFollowedUnit() const;
  const CVec2 &GetFollowShift() const { return vFollowShift; }

  // pFollowingUnit follows us
  void FollowingByYou(class CCommonUnit *pFollowingUnit);

  virtual void Segment();
  virtual void FreezeSegment();

  // Is it controllable (for example, a gun without crew is uncontrollable)
  virtual bool IsOperable() const { return true; }

  virtual const int GetMinArmor() const = 0;
  virtual const int GetMaxArmor() const = 0;
  virtual const int GetMinPossibleArmor(int nSide) const = 0;
  virtual const int GetMaxPossibleArmor(int nSide) const = 0;
  virtual const int GetArmor(int nSide) const = 0;
  virtual const int GetRandomArmor(int nSide) const = 0;

  virtual float GetMaxFireRange() const = 0;

  virtual void UnRegisterAsBored(enum EUnitAckType eBoredType) = 0;
  virtual void RegisterAsBored(enum EUnitAckType eBoredType) = 0;

  virtual EUnitAckType GetGunsRejectReason() const = 0;

  // for target resolution
  void SetShootEstimator(interface IShootEstimator *pShootEstimator);
  // reset all information in the shoot estimator and initialize it with the pCurEnemy unit
  // it is believed that we are now shooting at pCurEnemy, bDamageUpdated - was there an update on damage pCurEnemy by us
  void ResetShootEstimator(class CAIUnit *pCurEnemy, bool bDamageUpdated, DWORD dwForbidden = 0);
  void AddUnitToShootEstimator(class CAIUnit *pUnit);
  CAIUnit *GetBestShootEstimatedUnit() const;
  CBasicGun *GetBestShootEstimatedGun() const;
  const int GetNumOfBestShootEstimatedGun() const;

  virtual const float GetKillSpeed(class CAIUnit *pEnemy) const { return 0; }
  // reset time for scan periods
  virtual void ResetTargetScan() = 0;
  // scan if it's time; 
  virtual BYTE AnalyzeTargetScan(CAIUnit *pCurTarget, bool bDamageUpdated, bool bScanForObstacles, IRefCount *pCheckBuilding = nullptr) = 0;
  // scan for target
  virtual void LookForTarget(CAIUnit *pCurTarget, bool bDamageUpdated, CAIUnit **pBestTarget, CBasicGun **pGun) = 0;
  // search for obstacles
  virtual interface IObstacle *LookForObstacle() { return nullptr; };

  // is it necessary to try to drive close to the point that is guarded (for example, for trains it is not necessary)
  virtual bool CanMoveForGuard() const = 0;
  bool CanRotate() const override = 0;

  virtual bool CanCommandBeExecutedByStats(int nCmd) const = 0;

  virtual const NTimer::STime GetNextPathSegmTime() const { return 0; }

  virtual float GetPriceMax() const = 0;
  virtual const NTimer::STime &GetBehUpdateDuration() const = 0;

  virtual void AnimationSegment() {}

  bool IsDangerousDirExist() const override { return false; }
  const WORD GetDangerousDir() const override { return 0; }

  virtual const float GetTargetScanRadius() { return 0.0f; }

  bool CanBeFrozen() const;
  bool IsFrozenByState() const;
  void FreezeByState(bool bFreeze) override;

  virtual bool CanHookUnit(class CAIUnit *pUnitToHook) const { return false; }
  virtual bool IsTowing() const { return false; }

  virtual bool CanMoveAfterUserCommand() const = 0;
};

#endif // __COMMON_UNIT_H__