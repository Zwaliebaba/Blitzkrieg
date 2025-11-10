#ifndef __ENTRENCHMENT_H__
#define __ENTRENCHMENT_H__

#pragma once

#include "StaticObject.h"
#include "StormableObject.h"
#include "RotatingFireplacesObject.h"

class CSoldier;

// **********************************************************************
// *CEntrenchment*
// **********************************************************************

class CEntrenchment : public CStaticObject, public ILoadableObject, public CStormableObject, public CRotatingFireplacesObject
{
  OBJECT_COMPLETE_METHODS(CEntrenchment);
  DECLARE_SERIALIZE;

  struct SFireplaceInfo
  {
    DECLARE_SERIALIZE;

  public:
    CVec2 center;
    CPtr<CSoldier> pUnit;
    // segment number, description 
    int nFrameIndex;

    SFireplaceInfo() : nFrameIndex(-1) {}
    SFireplaceInfo(const CVec2 &_center, CSoldier *_pUnit, const int _nFrameIndex) : center(_center), pUnit(_pUnit), nFrameIndex(_nFrameIndex) {}
  };

  struct SInsiderInfo
  {
    DECLARE_SERIALIZE;

  public:
    CPtr<CSoldier> pUnit;
    // -1 if in reserve
    int nFireplace;

    SInsiderInfo() {}
    SInsiderInfo(CSoldier *_pUnit, const int _nFireplace) : pUnit(_pUnit), nFireplace(_nFireplace) {}
  };

  SRect rect;
  int z;

  int nBusyFireplaces;
  std::vector<SFireplaceInfo> fireplaces;

  std::list<SInsiderInfo> insiders;
  std::list<SInsiderInfo>::iterator iter;

  CGDBPtr<SEntrenchmentRPGStats> pStats;

  NTimer::STime nextSegmTime;

  //
  static CVec2 GetShift(const CVec2 &vPoint, const CVec2 &vDir);
  void ProcessEmptyFireplace(int nFireplace);

protected:
  void AddSoldier(CSoldier *pUnit) override;
  void AddSoldierToFirePlace(CSoldier *pUnit, int nFirePlace);
  void DelSoldier(CSoldier *pUnit, bool bFillEmptyFireplace) override;
  void SoldierDamaged(class CSoldier *pUnit) override {}

public:
  CEntrenchment() {}
  CEntrenchment(IRefCount **segments, int nLen, class CFullEntrenchment *pFullEntrenchment);

  const SHPObjectRPGStats *GetStats() const override { return pStats; }

  const CVec2 &GetCenter() const override { return rect.center; }
  const CVec2 GetAttackCenter(const CVec2 &vPoint) const override { return rect.center; }
  void GetCoveredTiles(CTilesSet *pTiles) const override;
  void GetBoundRect(SRect *pRect) const override { *pRect = rect; }
  bool IsPointInside(const CVec2 &point) const override { return rect.IsPointInside(point); }
  const WORD GetDir() const override { return GetDirectionByVector(rect.dir); }

  const float GetHitPoints() const override { return pStats->fMaxHP; }
  void TakeDamage(float fDamage, bool bFromExplosion, int nPlayerOfShoot, CAIUnit *pShotUnit) override;
  void TakeEditorDamage(const float fDamage) override {}

  void Segment() override;
  const NTimer::STime GetNextSegmentTime() const override { return nextSegmTime; }

  EStaticObjType GetObjectType() const override { return ESOT_ENTRENCHMENT; }

  // iterating over fire slots
  void StartIterate() override { iter = insiders.begin(); }
  void Iterate() override;
  bool IsIterateFinished() override { return iter == insiders.end(); }
  class CAIUnit *GetIteratedUnit() override;

  bool IsContainer() const override { return true; }
  const int GetNDefenders() const override;
  class CSoldier *GetUnit(int n) const override;
  const BYTE GetPlayer() const override;

  void Delete();

  // returns in pvResult the point in the trench closest to vPoint
  void GetClosestPoint(const CVec2 &vPoint, CVec2 *pvResult) const;
  const bool IsVisibleForDiplomacyUpdate() override { return IsAnyInsiderVisible(); }

  bool CanUnitGoThrough(const EAIClass &eClass) const override { return true; }

  // Is it possible to change the slot for this slot?
  bool CanRotateSoldier(class CSoldier *pSoldier) const override;
  // put the soldier in place instead of the one sitting there
  void ExchangeUnitToFireplace(class CSoldier *pSoldier, int nFirePlace) override;
  // number of fireplaces
  const int GetNFirePlaces() const override;
  // a soldier sitting in a fireplace, if the fireplace is empty then returns 0
  class CSoldier *GetSoldierInFireplace(int nFireplace) const override;
  //
  const CVec2 GetFirePlaceCoord(int nFirePlace);
};

// **********************************************************************
// *CEntrenchmentPart*
// **********************************************************************

class CFullEntrenchment;

class CEntrenchmentPart : public CExistingObject
{
  OBJECT_COMPLETE_METHODS(CEntrenchmentPart);
  DECLARE_SERIALIZE;

  CGDBPtr<SEntrenchmentRPGStats> pStats;
  CPtr<CEntrenchment> pOwner;// the trench where it belongs

  CVec2 center;
  WORD dir;

  SRect boundRect;

  bool bVisible;
  CTilesSet coveredTiles;
  CObj<CFullEntrenchment> pFullEntrenchment;

  NTimer::STime nextSegmTime;

  //
  // visible to all sides
  bool CanUnregister() const;
  //
  static CVec2 GetShift(const CVec2 &vPoint, const CVec2 &vDir);

protected:
  void SetNewPlaceWithoutMapUpdate(const CVec2 &_center, WORD _dir = 0) override;

public:
  CEntrenchmentPart() {}
  void Init();
  // nFrameIndex - index in the SEntrenchmentRPGStats::segments vector
  CEntrenchmentPart(const SEntrenchmentRPGStats *pStats, const CVec2 &center, WORD dir, int nFrameIndex, int dbID, float fHP);
  static SRect CalcBoundRect(const CVec2 &center, WORD _dir, const SEntrenchmentRPGStats::SSegmentRPGStats &stats);

  const SEntrenchmentRPGStats::SSegmentRPGStats &GetSegmStats() const { return pStats->segments[nFrameIndex]; }
  const SEntrenchmentRPGStats::EEntrenchSegmType GetType() const { return pStats->segments[nFrameIndex].eType; }
  CEntrenchment *GetOwner() const { return pOwner; }
  void SetOwner(CEntrenchment *_pOwner) { pOwner = _pOwner; }

  const CVec2 &GetCenter() const override { return center; }
  const CVec2 GetAttackCenter(const CVec2 &vPoint) const override { return boundRect.center; }
  const WORD GetDir() const override { return dir; }

  void GetRPGStats(struct SAINotifyRPGStats *pStats) override;

  void GetBoundRect(SRect *pRect) const override { *pRect = boundRect; }
  bool IsPointInside(const CVec2 &point) const override;
  void GetCoveredTiles(CTilesSet *pTiles) const override;

  void Segment() override;

  void LockTiles(bool bInitialization = false) override {}
  void UnlockTiles(bool bInitialization = false) override {}
  void SetTransparencies() override {}
  void RemoveTransparencies() override {}
  void RestoreTransparencies() override {}
  bool CanBeMovedTo(const CVec2 &newCenter) const override { return true; }

  const SHPObjectRPGStats *GetStats() const override { return pStats; }

  void TakeDamage(float fDamage, bool bFromExplosion, int nPlayerOfShoot, CAIUnit *pShotUnit) override;
  // immortal
  void Die(const float fDamage) override {}

  EStaticObjType GetObjectType() const override { return ESOT_ENTR_PART; }

  bool IsContainer() const override { return true; }
  const int GetNDefenders() const override { return pOwner->GetNDefenders(); }
  class CSoldier *GetUnit(const int n) const override { return pOwner->GetUnit(n); }

  void SetVisible();
  void SetFullEntrench(class CFullEntrenchment *_pFullEntrenchment) { pFullEntrenchment = _pFullEntrenchment; }

  bool CanUnitGoThrough(const EAIClass &eClass) const override { return true; }
};

// **********************************************************************
// *CFullEntrenchment*
// **********************************************************************

class CFullEntrenchment : public IRefCount
{
  OBJECT_COMPLETE_METHODS(CFullEntrenchment);
  DECLARE_SERIALIZE;

  std::list<CEntrenchmentPart *> entrenchParts;

public:
  CFullEntrenchment() {}

  void AddEntrenchmentPart(class CEntrenchmentPart *pEntrenchmentPart);
  void SetVisible();
};

// tank trench
class CEntrenchmentTankPit : public CGivenPassabilityStObject
{
  OBJECT_COMPLETE_METHODS(CEntrenchmentTankPit);
  DECLARE_SERIALIZE;

  CGDBPtr<SMechUnitRPGStats> pStats;
  WORD wDir;

  CVec2 vHalfSize;
  SRect boundRect;
  CTilesSet tilesToLock;

  CAIUnit *pOwner;

protected:
  void SetNewPlaceWithoutMapUpdate(const CVec2 &_center, const WORD _dir = 0) override {}

public:
  CEntrenchmentTankPit() {}
  // nFrameIndex - index in the SEntrenchmentRPGStats::segments vector
  CEntrenchmentTankPit(const SMechUnitRPGStats *pStats, const CVec2 &center, WORD dir, int nFrameIndex, int dbID, const class CVec2 &vResizeFactor, const CTilesSet &tilesToLock, class CAIUnit *_pOwner);

  const WORD GetDir() const override { return wDir; }

  void GetRPGStats(struct SAINotifyRPGStats *pStats) override {}
  void GetBoundRect(SRect *pRect) const override { *pRect = boundRect; }
  bool IsPointInside(const CVec2 &point) const override { return false; }
  void GetCoveredTiles(CTilesSet *pTiles) const override;
  void GetTilesForVisibility(CTilesSet *pTiles) const override;
  void Segment() override {}

  void LockTiles(bool bInitialization = false) override;
  void UnlockTiles(bool bInitialization = false) override;
  void SetTransparencies() override {}
  void RemoveTransparencies() override {}
  bool CanBeMovedTo(const CVec2 &newCenter) const override { return true; }
  const SHPObjectRPGStats *GetStats() const override { return pStats; }

  void TakeDamage(float fDamage, bool bFromExplosion, int nPlayerOfShoot, CAIUnit *pShotUnit) override;
  void Die(float fDamage) override;

  EStaticObjType GetObjectType() const override { return ESOT_TANKPIT; }

  void GetNewUnitInfo(struct SNewUnitInfo *pNewUnitInfo) override;

  bool IsContainer() const override { return false; }
  const int GetNDefenders() const override { return 0; }
  class CSoldier *GetUnit(const int n) const override { return nullptr; }

  bool CanUnitGoThrough(const EAIClass &eClass) const override { return false; }
  bool ShouldSuspendAction(const EActionNotify &eAction) const override;
};

#endif // __ENTRENCHMENT_H__