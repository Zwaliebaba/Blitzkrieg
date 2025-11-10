#ifndef __STATIC_OBJECT_H__
#define __STATIC_OBJECT_H__

#pragma once

#include "LinkObject.h"
#include "RectTiles.h"

enum EStaticObjType
{
  ESOT_COMMON,
  ESOT_BUILDING,
  ESOT_MINE,
  ESOT_ENTR_PART,
  ESOT_ENTRENCHMENT,
  ESOT_TERRA,
  ESOT_BRIDGE_SPAN,
  ESOT_TANKPIT,
  ESOT_FENCE,
  ESOT_SMOKE_SCREEN,
  ESOT_FLAG,
  ESOT_ARTILLERY_BULLET_STORAGE,
};

// **********************************************************************
// *CStaticObject*
// **********************************************************************

class CStaticObject : public CLinkObject
{
  DECLARE_SERIALIZE;

public:
  virtual const SHPObjectRPGStats *GetStats() const = 0;

  // object location
  virtual const CVec2 &GetCenter() const = 0;
  virtual const CVec2 GetAttackCenter(const CVec2 &vPoint) const = 0;
  virtual void GetBoundRect(SRect *pRect) const = 0;
  virtual void GetCoveredTiles(CTilesSet *pTiles) const = 0;
  virtual bool IsPointInside(const CVec2 &point) const = 0;
  virtual const WORD GetDir() const = 0;

  // hit points and damage
  virtual const float GetHitPoints() const = 0;
  virtual void SetHitPoints(const float fNewHP) { NI_ASSERT_T(false, "wrong call,CStaticObject::SetHitPoints"); }
  bool IsAlive() const override { return GetHitPoints() > 0.0f; }
  virtual void TakeDamage(float fDamage, bool bFromExplosion, int nPlayerOfShoot, CAIUnit *pShotUnit) = 0;
  virtual void TakeEditorDamage(float fDamage) = 0;

  // segment to handle some internal object logic
  virtual void Segment() = 0;
  virtual const NTimer::STime GetNextSegmentTime() const { return 0; }

  virtual EStaticObjType GetObjectType() const = 0;
  const BYTE GetPlayer() const override;
  virtual void SetPlayerForEditor(const int nPlayer) {}

  virtual bool IsContainer() const = 0;
  virtual const int GetNDefenders() const = 0;
  virtual class CSoldier *GetUnit(int n) const = 0;

  // for suspended updates
  const bool IsVisible(BYTE cParty) const override;
  void GetTilesForVisibility(CTilesSet *pTiles) const override;
  bool ShouldSuspendAction(const EActionNotify &eAction) const override;

  // true if the object can be added to this point
  static bool CheckStaticObject(const SObjectBaseRPGStats *pStats, const CVec2 &vPos, int nFrameIndex);

  // can an eClass unit drive through an object?
  virtual bool CanUnitGoThrough(const EAIClass &eClass) const = 0;
};

// **********************************************************************
// *ILoadableObject*
// **********************************************************************

// an object that can have units inside it
interface ILoadableObject
{
  // iterating over fire slots
  virtual void StartIterate() = 0;
  virtual void Iterate() = 0;
  virtual bool IsIterateFinished() = 0;
  virtual class CAIUnit *GetIteratedUnit() = 0;
};

// an object actually located on the map
class CExistingObject : public CStaticObject
{
  DECLARE_SERIALIZE;

  unsigned long mark;

  NTimer::STime burningEnd;

protected:
  static unsigned long globalMark;

  int dbID;
  int nFrameIndex;
  float fHP;

  //
  virtual void SetNewPlaceWithoutMapUpdate(const CVec2 &center, WORD dir = 0) = 0;
  const int GetRandArmorByDir(int nArmorDir, WORD wAttackDir);

public:
  CExistingObject() {}

  CExistingObject(const int _nFrameIndex, const int _dbID, const float _fHP)
    : mark(0), dbID(_dbID), nFrameIndex(_nFrameIndex), fHP(_fHP) { SetUniqueId(); }

  // object information
  const int GetDBID() const override { return dbID; }
  const int GetFrameIndex() const { return nFrameIndex; }

  // hit points and damage
  void TakeEditorDamage(float fDamage) override;
  const float GetHitPoints() const override { return fHP; }
  // performing the necessary actions when HitPoints goes to zero
  virtual void Die(float fDamage) = 0;

  // object location
  virtual void SetNewPlacement(const CVec2 &center, WORD dir);

  virtual void LockTiles(bool bInitialization = false) = 0;
  virtual void UnlockTiles(bool bInitialization = false) = 0;
  virtual void SetTransparencies() = 0;
  virtual void RemoveTransparencies() = 0;
  // set transparency again if it is currently set
  virtual void RestoreTransparencies() = 0;

  virtual bool CanBeMovedTo(const CVec2 &newCenter) const = 0;

  // for updater
  void GetPlacement(struct SAINotifyPlacement *pPlacement, NTimer::STime timeDiff) override;
  void GetNewUnitInfo(struct SNewUnitInfo *pNewUnitInfo) override;
  void GetRPGStats(struct SAINotifyRPGStats *pStats) override = 0;

  virtual void Delete();
  void DeleteForEditor();

  // for iterator
  virtual bool IsGlobalUpdated() const { return mark == globalMark; }
  virtual void SetGlobalUpdated() { mark = globalMark; }
  static void UpdateGlobalMark() { ++globalMark; }

  // true on hit
  virtual bool ProcessCumulativeExpl(class CExplosion *pExpl, int nArmorDir, bool bFromExpl);
  virtual bool ProcessBurstExpl(class CExplosion *pExpl, int nArmorDir, float fRadius, float fSmallRadius);
  virtual bool ProcessAreaDamage(const class CExplosion *pExpl, const int nArmorDir, const float fRadius, const float fSmallRadius) { return false; }

  // segment for burning object
  virtual void BurnSegment();
  // hitting an object with damage (call only after writing off damage)
  virtual void WasHit();

  friend class CStaticMembers;
};

// **********************************************************************
// *CGivenPassabilityStObject*
// **********************************************************************

// object with a given cross-country ability
class CGivenPassabilityStObject : public CExistingObject
{
  DECLARE_SERIALIZE;
  CVec2 center;
  SRect boundRect;
  CArray2D<BYTE> lockInfo;
  BYTE lockTypes;
  bool bPartially;
  bool bTransparencySet;

  CArray2D<BYTE> canSetTransparency;

  //
  const int GetVisDownX() const;
  const int GetVisUpX() const;
  const int GetVisDownY() const;
  const int GetVisUpY() const;

protected:
  void SetNewPlaceWithoutMapUpdate(const CVec2 &_center, const WORD dir = 0) override { center = _center; }
  void InitTransparenciesPossibility();

public:
  CGivenPassabilityStObject() : bTransparencySet(false) {}
  CGivenPassabilityStObject(const CVec2 &center, int dbID, float fHP, int nFrameIndex);
  virtual void Init();

  const CVec2 &GetCenter() const override { return center; }
  const CVec2 GetAttackCenter(const CVec2 &vPoint) const override;
  const WORD GetDir() const override { return 0; }

  void GetRPGStats(struct SAINotifyRPGStats *pStats) override;

  virtual const int GetDownX() const;
  virtual const int GetUpX() const;
  virtual const int GetDownY() const;
  virtual const int GetUpY() const;

  void GetCoveredTiles(CTilesSet *pTiles) const override;
  void GetBoundRect(SRect *pRect) const override { *pRect = boundRect; }
  bool IsPointInside(const CVec2 &point) const override;

  void LockTiles(bool bInitialization = false) override;
  void UnlockTiles(bool bInitialization = false) override;
  void SetTransparencies() override;
  void RemoveTransparencies() override;
  void RestoreTransparencies() override;
  bool CanBeMovedTo(const CVec2 &newCenter) const override;
};

// **********************************************************************
// *CCommonStaticObject*
// **********************************************************************

class CCommonStaticObject : public CGivenPassabilityStObject
{
  DECLARE_SERIALIZE;

  EStaticObjType eType;

public:
  CCommonStaticObject() {}

  CCommonStaticObject(const CVec2 &center, const int dbID, const float fHP, const int nFrameIndex, EStaticObjType _eType)
    : CGivenPassabilityStObject(center, dbID, fHP, nFrameIndex), eType(_eType) {}

  void TakeDamage(float fDamage, bool bFromExplosion, int nPlayerOfShoot, CAIUnit *pShotUnit) override;
  void Die(float fDamage) override;

  void Segment() override {}

  EStaticObjType GetObjectType() const override { return eType; }

  bool IsContainer() const override { return false; }
  const int GetNDefenders() const override { return 0; }
  class CSoldier *GetUnit(const int n) const override { return nullptr; }

  bool ProcessAreaDamage(const class CExplosion *pExpl, int nArmorDir, float fRadius, float fSmallRadius) override;
};

class CSimpleStaticObject : public CCommonStaticObject
{
  OBJECT_COMPLETE_METHODS(CSimpleStaticObject);
  DECLARE_SERIALIZE;

  CGDBPtr<SStaticObjectRPGStats> pStats;
  int nPlayer;
  bool bDelayedUpdate;// if true then update for new object is delayed
public:
  CSimpleStaticObject() {}

  CSimpleStaticObject(const SStaticObjectRPGStats *_pStats, const CVec2 &center, const int dbID, const float fHP, const int nFrameIndex, EStaticObjType eType, const int nPlayer = -1, const bool bDelayedUpdate = false)
    : CCommonStaticObject(center, dbID, fHP, nFrameIndex, eType), pStats(_pStats), nPlayer(nPlayer), bDelayedUpdate(bDelayedUpdate) {}

  const BYTE GetPlayer() const override
  {
    if (-1 == nPlayer) return CCommonStaticObject::GetPlayer();
    return nPlayer;
  }

  const SHPObjectRPGStats *GetStats() const override { return pStats; }
  bool CanUnitGoThrough(const EAIClass &eClass) const override;
  bool ShouldSuspendAction(const EActionNotify &eAction) const override;
};

class CTerraMeshStaticObject : public CCommonStaticObject
{
  OBJECT_COMPLETE_METHODS(CTerraMeshStaticObject);
  DECLARE_SERIALIZE;

  CGDBPtr<SStaticObjectRPGStats> pStats;
  WORD wDir;

public:
  CTerraMeshStaticObject() {}

  CTerraMeshStaticObject(const SStaticObjectRPGStats *_pStats, const CVec2 &center, const WORD _wDir, const int dbID, const float fHP, const int nFrameIndex, EStaticObjType eType)
    : CCommonStaticObject(center, dbID, fHP, nFrameIndex, eType), pStats(_pStats), wDir(_wDir) {}

  const SHPObjectRPGStats *GetStats() const override { return pStats; }
  bool CanUnitGoThrough(const EAIClass &eClass) const override;

  const WORD GetDir() const override { return wDir; }

  void SetNewPlacement(const CVec2 &center, WORD dir) override;
  void GetPlacement(struct SAINotifyPlacement *pPlacement, NTimer::STime timeDiff) override;
};

#endif // __STATIC_OBJECT_H__