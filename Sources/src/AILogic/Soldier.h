#ifndef __SOLDIER_H__
#define __SOLDIER_H__

#pragma once
// ///////////////////////////////////////////////////////////// 
#include "AIUnit.h"
#include "AIWarFog.h"
#include "StaticObjectSlotInfo.h"
// ///////////////////////////////////////////////////////////// 
class CUnitGuns;
class CFormation;

// ///////////////////////////////////////////////////////////// 
class CSoldier : public CAIUnit
{
  DECLARE_SERIALIZE;

  enum EObjectInsideOf { EOIO_NONE, EOIO_BUILDING, EOIO_TRANSPORT, EOIO_ENTRENCHMENT, EOIO_UNKNOWN };

  EObjectInsideOf eInsideType;

  CGDBPtr<SInfantryRPGStats> pStats;
  // gun barrels
  CPtr<CUnitGuns> pGuns;

  IRefCount *pObjInside;

  SStaticObjectSlotInfo slotInfo;

  WORD wMinAngle, wMaxAngle;
  float fOwnSightRadius;

  bool bInFirePlace, bInSolidPlace;

  NTimer::STime lastHit, lastCheck;
  NTimer::STime lastMineCheck;// final check min (for engineers)
  NTimer::STime lastDirUpdate;
  bool bLying;

  CPtr<CFormation> pFormation;
  CPtr<CFormation> pMemorizedFormation;
  CPtr<CFormation> pVirtualFormation;
  BYTE cFormSlot;
  bool bWait2Form;

  bool bAllowLieDown;// can a soldier lie down under fire (or stand like a tin one.)
  NTimer::STime nextSegmTime;
  NTimer::STime timeBWSegments;
  NTimer::STime nextPathSegmTime;
  NTimer::STime nextLogicSegmTime;

  //
  void UpdateLyingPosition();

protected:
  void PrepareToDelete() override;

  void InitGuns() override;
  // shows all mines that are within the detection radius of this engineer
  // 
  virtual void RevealNearestMines(bool bIncludingAP);

  bool CalculateUnitVisibility4Party(BYTE cParty) const override;

public:
  CSoldier() : eInsideType(EOIO_NONE), pObjInside(nullptr), wMinAngle(0), wMaxAngle(0), fOwnSightRadius(-1), bInFirePlace(false), bInSolidPlace(false), pFormation(nullptr), cFormSlot(0) {}

  void Init(const CVec2 &center, int z, const SUnitBaseRPGStats *pStats, float fHP, WORD dir, BYTE player, WORD id, EObjVisType eVisType, int dbID) override;

  const SUnitBaseRPGStats *GetStats() const override { return pStats; }
  IStatesFactory *GetStatesFactory() const override;

  void UpdateDirection(const CVec2 &newDir) override;
  void UpdateDirection(WORD newDir) override;

  virtual void AllowLieDown(bool _bAllowLieDown);
  bool IsAllowedToLieDown() const { return bAllowLieDown; }
  void MoveToEntrenchFireplace(const CVec3 &coord, int _nSlot);

  class CBuilding *GetBuilding() const;
  class CEntrenchment *GetEntrenchment() const;
  class CMilitaryCar *GetTransportUnit() const;

  void SetInBuilding(class CBuilding *pBuilding);
  void SetInEntrenchment(class CEntrenchment *pEntrenchment);
  void SetInTransport(class CMilitaryCar *pUnit);
  void SetFree();

  bool IsInSolidPlace() const override;
  bool IsInFirePlace() const override;
  void SetToFirePlace();
  void SetToSolidPlace();
  bool IsFree() const override;

  void SetNSlot(const int nSlot) { slotInfo.nSlot = nSlot; }
  const int GetSlot() const { return slotInfo.nSlot; }
  const SStaticObjectSlotInfo &GetSlotInfo() const { return slotInfo; }
  SStaticObjectSlotInfo &GetSlotInfo() { return slotInfo; }

  void SetSlotInfo(const int nSlot, const int nType, const int nIndex)
  {
    slotInfo.nSlot = nSlot;
    slotInfo.nType = nType;
    slotInfo.nIndex = nIndex;
  }

  void SetSlotIndex(const int nIndex) { slotInfo.nIndex = nIndex; }
  const int GetSlotIndex() const { return slotInfo.nIndex; }

  void SetAngles(const WORD _wMinAngle, const WORD _wMaxAngle)
  {
    wMinAngle = _wMinAngle;
    wMaxAngle = _wMaxAngle;
  }

  WORD GetMinAngle() const { return wMinAngle; }
  WORD GetMaxAngle() const { return wMaxAngle; }
  bool IsAngleLimited() const;

  bool IsInBuilding() const { return eInsideType == EOIO_BUILDING; }
  bool IsInEntrenchment() const { return eInsideType == EOIO_ENTRENCHMENT; }
  bool IsInTransport() const { return eInsideType == EOIO_TRANSPORT; }

  // for shooting - fill in the field typeID, pUnit and slot number, if necessary
  void GetShotInfo(struct SAINotifyInfantryShot *pShotInfo) const override;
  // to throw grenades - fill in the field typeID, pUnit and slot number, if necessary
  void GetThrowInfo(struct SAINotifyInfantryShot *pThrowInfo) const;
  void GetEntranceStateInfo(struct SAINotifyEntranceState *pInfo) const override;

  const EActionNotify GetAimAction() const override;
  const EActionNotify GetShootAction() const override;
  virtual const EActionNotify GetThrowAction() const;
  const EActionNotify GetDieAction() const override;
  const EActionNotify GetIdleAction() const override;
  const EActionNotify GetMovingAction() const override;

  //
  bool CanMove() const override { return IsFree() && GetBehaviour().moving != SBehaviour::EMHoldPos; }
  bool CanMovePathfinding() const override { return IsFree(); }

  bool InVisCone(const CVec2 &point) const override;
  const float GetSightRadius() const override;
  void SetOwnSightRadius(const float _fOwnSightRadius) { fOwnSightRadius = _fOwnSightRadius; }
  void RemoveOwnSightRadius() { fOwnSightRadius = -1; }
  // the probability with which damage will be caused when hit
  const float GetCover() const override;
  bool IsLying() const { return bLying; }
  void LieDown();
  void StandUp();
  float GetSightMultiplier() const override;

  void Segment() override;
  void FreezeSegment() override;

  // formation
  void SetFormation(class CFormation *pFormation, BYTE cFormSlot);
  bool IsInFormation() const override { return pFormation != nullptr; }
  class CFormation *GetFormation() const override { return pFormation; }
  const CVec2 GetUnitPointInFormation() const override;
  const int GetFormationSlot() const override { return cFormSlot; }
  const bool CanShootToPlanes() const override;

  //
  int GetNGuns() const override;
  class CBasicGun *GetGun(int n) const override;
  class CTurret *GetTurret(int nTurret) const override;
  const int GetNTurrets() const override;
  int GetNAmmo(int nCommonGun) const override;
  void Fired(float fGunRadius, int nGun) override;
  // nAmmo with sign
  void ChangeAmmo(int nCommonGun, int nAmmo) override;
  bool IsCommonGunFiring(int nCommonGun) const override;

  class CBasicGun *ChooseGunForStatObj(class CStaticObject *pObj, NTimer::STime *pTime) override;

  const float GetMaxPossibleSpeed() const override;

  void GetFogInfo(SFogInfo *pInfo) const override;
  void ChangeWarFogState();

  void GetShootAreas(struct SShootAreas *pShootAreas, int *pnAreas) const override;
  float GetMaxFireRange() const override;

  bool IsMech() const override { return false; }

  bool IsNoticableByUnit(class CCommonUnit *pUnit, float fNoticeRadius) override;

  bool ProcessAreaDamage(const class CExplosion *pExpl, int nArmorDir, float fRadius, float fSmallRadius) override;

  const float GetDispersionBonus() const override;
  const float GetRelaxTimeBonus() const override;
  const float GetFireRateBonus() const override;

  virtual void MemCurFormation();
  class CFormation *GetMemFormation() { return pMemorizedFormation; }
  void SetVirtualFormation(class CFormation *pFormation);

  void SetWait2FormFlag(bool bNewValue) { bWait2Form = bNewValue; }
  bool IsInWait2Form() const { return bWait2Form; }

  bool IsColliding() const override;

  void MemorizeFormation();

  const int GetMinArmor() const override;
  const int GetMaxArmor() const override;
  const int GetMinPossibleArmor(int nSide) const override;
  const int GetMaxPossibleArmor(int nSide) const override;
  const int GetArmor(int nSide) const override;
  const int GetRandomArmor(int nSide) const override;

  const int CanGoBackward() const override { return false; }

  bool ShouldSuspendAction(const EActionNotify &eAction) const override;
  virtual bool CanJoinToFormation() const;

  // search for target, current target for attack - pCurTarget
  void LookForTarget(CAIUnit *pCurTarget, bool bDamageUpdated, CAIUnit **pBestTarget, class CBasicGun **pGun) override;
  void TakeDamage(float fDamage, const SWeaponRPGStats::SShell *pShell, int nPlayerOfShoot, CAIUnit *pShotUnit) override;

  const NTimer::STime GetNextSegmTime() const override { return nextSegmTime; }
  void NullSegmTime() override { timeBWSegments = 0; }

  void FirstSegment() override;
  const NTimer::STime GetNextPathSegmTime() const override { return nextPathSegmTime; }

  // number of segments that have passed since the last SecondSegment call
  const float GetPathSegmentsPeriod() const override;

  const NTimer::STime &GetBehUpdateDuration() const override { return SConsts::SOLDIER_BEH_UPDATE_DURATION; }

  class CArtillery *GetArtilleryIfCrew() const override;

  const CUnitGuns *GetGuns() const override { return pGuns; }
  CUnitGuns *GetGuns() override { return pGuns; }

  void FreezeByState(bool bFreeze) override;
};

// ///////////////////////////////////////////////////////////// 
class CInfantry : public CSoldier
{
  OBJECT_COMPLETE_METHODS(CInfantry);
  DECLARE_SERIALIZE;

public:
  CInfantry() {}
};

// ///////////////////////////////////////////////////////////// 
class CSniper : public CSoldier
{
  OBJECT_COMPLETE_METHODS(CSniper);
  DECLARE_SERIALIZE;

  NTimer::STime lastVisibilityCheck;
  // Is it visible to the opposite party?
  bool bVisible;
  // is it in sneak mode?
  bool bSneak;
  // probability of removing camouflage when shooting if we are in sneak mode
  float fCamouflageRemoveWhenShootProbability;

protected:
  bool CalculateUnitVisibility4Party(BYTE cParty) const override;

public:
  CSniper() : lastVisibilityCheck(0), bVisible(false), bSneak(false) {}

  void Init(const CVec2 &center, int z, const SUnitBaseRPGStats *pStats, float fHP, WORD dir, BYTE player, WORD id, EObjVisType eVisType, int dbID) override;

  void Segment() override;

  void SetVisible() { bVisible = true; }

  void RemoveCamouflage(ECamouflageRemoveReason eReason) override;
  void SetSneak(bool bSneakMode);

  void Fired(float fGunRadius, int nGun) override;
};

extern CGlobalWarFog theWarFog;

struct SSniperTrace
{
  const SVector centerTile;
  const bool bCamouflated;
  const float fCamouflage;
  const int nParty;
  CSniper *pSniper;

  //
  SSniperTrace(CSniper *_pSniper)
    : centerTile(_pSniper->GetTile().x, _pSniper->GetTile().y),
      bCamouflated(_pSniper->IsCamoulflated()), fCamouflage(_pSniper->GetCamouflage()),
      nParty(_pSniper->GetParty()),
      pSniper(_pSniper) {}

  bool CanTraceRay(const SVector &point) const { return true; }

  bool VisitPoint(const SVector &point, const int vis, const float fLen2, const float fR2, const float fSightPower2)
  {
    if (point.x == centerTile.x && point.y == centerTile.y)
    {
      if (!bCamouflated)
      {
        pSniper->SetVisible();
        return false;
      }
      const float fRatio = fSightPower2 * fLen2 / fR2;
      if (fRatio <= sqr(fCamouflage * static_cast<float>(theWarFog.GetTileVis(point, nParty)) / static_cast<float>(SConsts::VIS_POWER)))
      {
        pSniper->SetVisible();
        return false;
      }
    }

    return true;
  }
};

// ///////////////////////////////////////////////////////////// 
#endif // __SOLDIER_H__