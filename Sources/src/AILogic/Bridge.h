#ifndef __BRIDGE_H__
#define __BRIDGE_H__

#pragma once

#include "StaticObject.h"

class CFullBridge;

class CBridgeSpan : public CGivenPassabilityStObject
{
  OBJECT_COMPLETE_METHODS(CBridgeSpan);
  DECLARE_SERIALIZE;

  CGDBPtr<SBridgeRPGStats> pStats;

  CArray2D<BYTE> unlockTypes;// unlocked terrain types, 0 - if there was nothing to unlock
  CObj<CFullBridge> pFullBridge;
  bool bNewBuilt;// this bridge was built during the tgra
  bool bLocked;// are the tiles locked?

  // When a given segment dies, it begins to delete everything around it.
  bool bDeletingAround;

  int nScriptID;

  void GetTilesForVisibilityInternal(CTilesSet *pTiles) const;

  const int GetDownX() const override;
  const int GetUpX() const override;
  const int GetDownY() const override;
  const int GetUpY() const override;

public:
  CBridgeSpan() : nScriptID(-1) {}
  CBridgeSpan(const SBridgeRPGStats *pStats, const CVec2 &center, int dbID, float fHP, int nFrameIndex);

  void Build();// build a bridge segment, lock it as expected, send it into the world.

  const SBridgeRPGStats *GetBridgeStats() const { return pStats; }
  const SHPObjectRPGStats *GetStats() const override { return pStats; }

  void LockTiles(bool bInitialization = false) override;
  void UnlockTiles(bool bInitialization = false) override;

  void TakeDamage(float fDamage, bool bFromExplosion, int nPlayerOfShoot, CAIUnit *pShotUnit) override;
  void TakeEditorDamage(float fDamage) override;
  bool IsPointInside(const CVec2 &point) const override;
  void Die(float fDamage) override;
  void SetHitPoints(float fNewHP) override;


  void Segment() override {}

  EStaticObjType GetObjectType() const override { return ESOT_BRIDGE_SPAN; }

  bool IsContainer() const override { return false; }
  const int GetNDefenders() const override { return 0; }
  class CSoldier *GetUnit(const int n) const override { return nullptr; }

  void SetFullBrige(CFullBridge *pFullBridge);
  CFullBridge *GetFullBridge() { return pFullBridge; }

  bool CanUnitGoThrough(const EAIClass &eClass) const override { return true; }
  void GetTilesForVisibility(CTilesSet *pTiles) const override;
  bool ShouldSuspendAction(const EActionNotify &eAction) const override;
  void GetCoveredTiles(CTilesSet *pTiles) const override;

  void SetScriptID(const int _nScriptID) override { nScriptID = _nScriptID; }

  friend class CFullBridge;
};

class CFullBridge : public IRefCount
{
  OBJECT_COMPLETE_METHODS(CFullBridge);
  DECLARE_SERIALIZE;

  std::list<CBridgeSpan *> spans;// constructed parts of the bridge
  std::list<CBridgeSpan *> projectedSpans;// parts of the bridge that are in the project

  bool bGivingDamage;

public:
  struct SSpanLock : IRefCount
  {
    OBJECT_COMPLETE_METHODS(SSpanLock);
    DECLARE_SERIALIZE;

    CTilesSet tiles;
    std::list<BYTE> formerTiles;
    CBridgeSpan *pSpan;

  public:
    //
    SSpanLock() : pSpan(nullptr) {}
    SSpanLock(CBridgeSpan *pSpan, WORD wDir);
    void Unlock();
    const CBridgeSpan *GetSpan() const { return pSpan; }
  };

private:
  using LockedSpans = std::list<CPtr<SSpanLock>>;
  LockedSpans lockedSpans;
  int nSpans;// full number of bridge spans

public:
  CFullBridge() : bGivingDamage(false), nSpans(0) {}

  const float GetHPPercent() const;

  // when span was built
  void SpanBuilt(CBridgeSpan *pSpan);

  void AddSpan(CBridgeSpan *pSpan);
  void DamageTaken(CBridgeSpan *pDamagedSpan, float fDamage, bool bFromExplosion, int nPlayerOfShoot, CAIUnit *pShotUnit);

  void EnumSpans(std::vector<CObj<CBridgeSpan>> *pSpans);
  virtual void GetTilesForVisibility(CTilesSet *pTiles) const;
  const bool IsVisible(BYTE cParty) const;

  void LockSpan(CBridgeSpan *pSpan, WORD wDir);
  void UnlockSpan(CBridgeSpan *pSpan);
  void UnlockAllSpans();

  bool CanTakeDamage() const;
  const int GetNSpans() const;

  friend class CBridgeCreation;
};

#endif // __BRIDGE_H__