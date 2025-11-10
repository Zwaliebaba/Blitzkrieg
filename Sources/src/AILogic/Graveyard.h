#ifndef __GRAVEYARD_H__
#define __GRAVEYARD_H__

#pragma once

#include "LinkObject.h"

class CAIUnit;
class CCommonUnit;
interface IUpdatableObj;

struct SKilledUnit
{
  DECLARE_SERIALIZE;

public:
  CObj<CAIUnit> pUnit;
  NTimer::STime endFogTime;
  NTimer::STime endSceneTime;
  NTimer::STime timeToEndDieAnimation;
  // sent back to you like dead
  bool bSentDead;
  // The death animation has ended and endSceneTime and endFogTime have been initialized
  bool bAnimFinished;
  //
  bool bDisappearUpdateSent;

  bool bFatality;// Should I play Fatality?
  NTimer::STime actionTime;// action time for fatality
  CTilesSet lockedTiles;// locked tiles for fatalities

  bool bFogDeleted;

  SKilledUnit() : bFogDeleted(false) {}
  SKilledUnit(CAIUnit *_pUnit, const NTimer::STime _timeToEndDieAnimation) : pUnit(_pUnit), endFogTime(0), endSceneTime(0), timeToEndDieAnimation(_timeToEndDieAnimation), bSentDead(false), bAnimFinished(false), bDisappearUpdateSent(false), bFatality(false), actionTime(0), bFogDeleted(false) {}
  SKilledUnit(CAIUnit *_pUnit, const NTimer::STime _timeToEndDieAnimation, const NTimer::STime _endSceneTime, const NTimer::STime _endFogTime) : pUnit(_pUnit), endFogTime(_endFogTime), endSceneTime(_endSceneTime), timeToEndDieAnimation(_timeToEndDieAnimation), bSentDead(false), bAnimFinished(false), bDisappearUpdateSent(false), bFatality(false), actionTime(0), bFogDeleted(false) {}
};

class CDeadUnit : public CLinkObject
{
  OBJECT_COMPLETE_METHODS(CDeadUnit);
  DECLARE_SERIALIZE;

  CPtr<IUpdatableObj> pDieObj;
  NTimer::STime dieTime;
  EActionNotify dieAction;
  int nFatality;
  bool bPutMud;

  SVector tileCenter;

public:
  CDeadUnit() {}
  CDeadUnit(class CCommonUnit *_pDieObj, NTimer::STime _dieTime, EActionNotify _dieAction, bool bPutMud);
  CDeadUnit(class CCommonUnit *_pDieObj, NTimer::STime _dieTime, EActionNotify _dieAction, int _nFatality, bool bPutMud);

  void GetDyingInfo(struct SAINotifyAction *pDyingInfo) override;

  const bool IsVisible(BYTE cParty) const override;
  void GetTilesForVisibility(CTilesSet *pTiles) const override;
  bool ShouldSuspendAction(const EActionNotify &eAction) const override;

  IUpdatableObj *GetDieObject() const override;
};

class CGraveyard
{
  DECLARE_SERIALIZE;

  std::list<SKilledUnit> killed;
  using UpdateObjSet = std::hash_map<CObj<CAIUnit>, float, SUnitObjHash>;
  UpdateObjSet soonBeDead;

  using CBridgeDeadSoldiers = std::hash_map<int, std::list<CPtr<CDeadUnit>>>;
  std::hash_set<IUpdatableObj *, SDefaultPtrHash> bridgeSoldiersSet;
  CBridgeDeadSoldiers bridgeDeadSoldiers;

  //
  void CheckSoonBeDead();

public:
  void Segment();

  void GetDeadUnits(SAINotifyDeadAtAll **pDeadUnitsBuffer, int *pnLen);

  void AddKilledUnit(class CAIUnit *pUnit, const NTimer::STime &timeOfVisDeath, int nFatality);
  void PushToKilled(const SKilledUnit &killedUnit, CAIUnit *pUnit);
  void AddToSoonBeDead(class CAIUnit *pUnit, float fDamage);
  void DelKilledUnitsFromRect(const SRect &rect, CAIUnit *pShotUnit);

  void AddBridgeKilledSoldier(const SVector &tile, CAIUnit *pSoldier);
  void FreeBridgeTile(const SVector &tile);

  void UpdateFog4RemovedObject(class CExistingObject *pObj);
  void UpdateFog4AddedObject(class CExistingObject *pObj);

  void Clear();
};

#endif // __GRAVEYARD_H__