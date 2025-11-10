#ifndef __BUILDING_H__
#define __BUILDING_H__

#pragma once

#include "StaticObject.h"
#include "Heap.h"
#include "StormableObject.h"
#include "RotatingFireplacesObject.h"

class CSoldier;
class CCommonUnit;
class CTurret;
class CUnitGuns;

// **********************************************************************
// *CBuilding*
// **********************************************************************

class CBuilding : public CGivenPassabilityStObject, public ILoadableObject, public CStormableObject
{
  DECLARE_SERIALIZE;

  // the beginning of a state when everything is in arrow. 
  NTimer::STime startOfRest;
  // anxiety
  bool bAlarm;

  struct SHealthySort
  {
    bool operator()(const CPtr<CSoldier> &a, const CPtr<CSoldier> &b);
  };

  struct SIllSort
  {
    bool operator()(const CPtr<CSoldier> &a, const CPtr<CSoldier> &b);
  };

  struct SSwapAction
  {
    void operator()(CPtr<CSoldier> pSoldier1, CPtr<CSoldier> pSoldier2, int nSoldier1Index, int nSoldier2Index);
  };

  CHeap<CPtr<CSoldier>, SHealthySort, SSwapAction> medical;
  CHeap<CPtr<CSoldier>, SIllSort, SSwapAction> fire;
  CHeap<CPtr<CSoldier>, SIllSort, SSwapAction> rest;
  int nOveralPlaces;

  int nIterator;

  CCommonUnit *pLockingUnit;

  NTimer::STime nextSegmTime;

  std::vector<CObj<CTurret>> turrets;
  std::vector<CPtr<CUnitGuns>> guns;

  NTimer::STime lastDistibution;

  // 3 observation fireplaces for each side
  CArray2D<int> observationPlaces;

  struct SSideInfo
  {
    // number of fireSlots on the side
    int nFireSlots;
    // number of observation points on the side
    int nObservationPoints;
    // number of soldiers at observation points side
    int nSoldiersInObservationPoints;

    SSideInfo() : nFireSlots(0), nObservationPoints(0), nSoldiersInObservationPoints(0) {}
  };

  std::vector<SSideInfo> sides;
  // by fire place - <point number << 2> | 
  std::vector<int> firePlace2Observation;
  // by fireplace - there is a soldier in it
  std::vector<CPtr<CSoldier>> firePlace2Soldier;
  int nLastFreeFireSoldierChoice;

  // player of the last defender to enter the building
  int nLastPlayer;

  int nScriptID;

  // should units run away from a building when it is low on health?
  bool bShouldEscape;
  // units escaped
  bool bEscaped;
  NTimer::STime timeOfDeath;

  std::vector<NTimer::STime> lastLeave;

  //
  bool IsIllInFire();
  bool IsIllInRest();

  void SwapFireMed();
  void SwapRestMed();

  const BYTE GetFreeFireSlot();

  // put a specific unit into a slot
  void PopFromFire();

  // There is a free slot, put anyone there
  void SeatSoldierToMedicalSlot();
  // there is a free slot, put a volunteer there
  void SeatSoldierToFireSlot();

  // redistribute soldiers (treat/expel them from medical places)
  void DistributeAll();
  // redistribute soldiers who are not shooting (treat/expel them from medical places)
  void DistributeNonFires();

  void SetFiringUnitProperties(class CSoldier *pUnit, int nSlot, int nIndex);
  void DistributeFiringSoldiers();

  void InitObservationPlaces();

  //
  void DelSoldierFromFirePlace(CSoldier *pSoldier);
  void DelSoldierFromMedicalPlace(CSoldier *pSoldier);
  void DelSoldierFromRestPlace(CSoldier *pSoldier);

  void PushSoldierToFirePlace(CSoldier *pUnit, int nFirePlace);

  // put pUnit in the first available fireplace
  void PushToFire(class CSoldier *pUnit);
  void PushToMedical(class CSoldier *pUnit);
  void PushToRest(class CSoldier *pUnit);

  // place soldiers at observation points
  void SetSoldiersToObservationPoints();
  // try to place the resting soldier pSoldier at an observation point; 
  bool TryToPushRestSoldierToObservation(CSoldier *pSoldier);
  // try to place a soldier in a shooting cell at an observation point; 
  bool TryToPushFireSoldierToObservation(CSoldier *pSoldier);

  // whether pSoldier is at the observation point
  bool IsSoldierInObservationPoint(CSoldier *pSoldier) const;
  // put pSoldier in the observation point on the nSide side
  void PushSoldierToObservationPoint(CSoldier *pSoldier, int nSide);

  // return nSide side viewpoints
  void GetSidesObservationPoints(int nSide, int *pnLeftPoint, int *pnRightPoint) const;
  // return the central viewpoint of the nSide side
  const int GetMiddleObservationPoint(int nSide) const;
  // return the first of the soldiers on the nSize side
  CSoldier *GetSoldierOnSide(int nSide);
  // true, if pSoldierInPoint in observation point is better to change to pSoldier
  bool IsBetterChangeObservationSoldier(CSoldier *pSoldier, CSoldier *pSoldierInPoint);
  // choose a side to place the soldier at the observation point,
  // if there is already a soldier at each point, rotate -1
  const int ChooseSideToSetSoldier(class CSoldier *pSoldier) const;
  void CentreSoldiersInObservationPoints();
  // put soldiers behind built-in cannons
  void ExchangeSoldiersToTurrets();

  // the amount of HP when it’s time to run away from the building (run away if there are fewer of them)
  const float GetEscapeHitPoints() const;
  // kick a soldier's formation out of the house, pFormations - list of already kicked out formations
  void DriveOut(CSoldier *pSoldier, std::hash_set<int> *pFormations);
  void KillAllInsiders();

protected:
  CGDBPtr<SBuildingRPGStats> pStats;

  void AddSoldier(class CSoldier *pUnit) override;
  void DelSoldier(class CSoldier *pUnit, bool bFillEmptyFireplace) override;
  void SoldierDamaged(class CSoldier *pUnit) override;

  CBuilding() : pLockingUnit(nullptr), nextSegmTime(0), nScriptID(-1) {}
  CBuilding(const SBuildingRPGStats *pStats, const CVec2 &center, int dbID, float fHP, int nFrameIndex);

public:
  const int GetNFreePlaces() const;
  const int GetNOverallPlaces() const { return nOveralPlaces; }

  const SHPObjectRPGStats *GetStats() const override { return pStats; }
  void SetHitPoints(float fNewHP) override;
  void TakeDamage(float fDamage, bool bFromExplosion, int nPlayerOfShoot, CAIUnit *pShotUnit) override;
  void Die(float fDamage) override;

  const int GetNEntrancePoints() const { return pStats->entrances.size(); }
  const CVec2 GetEntrancePoint(int nEntrance) const;
  // find the exit closest to go to point
  bool ChooseEntrance(class CCommonUnit *pUnit, const CVec2 &vPoint, int *pnEntrance) const;

  void GoOutFromEntrance(int nEntrance, class CSoldier *pUnit);
  bool IsGoodPointForRunIn(const SVector &point, int nEntrance, float fMinDist = 0) const;

  void Segment() override;
  const NTimer::STime GetNextSegmentTime() const override { return nextSegmTime; }

  EStaticObjType GetObjectType() const override { return ESOT_BUILDING; }

  // iterating over fire slots
  void StartIterate() override { nIterator = 0; }
  void Iterate() override { if (nIterator < fire.Size()) ++nIterator; }
  bool IsIterateFinished() override { return nIterator == fire.Size(); }
  class CAIUnit *GetIteratedUnit() override;

  bool IsContainer() const override { return true; }
  const int GetNDefenders() const override;
  class CSoldier *GetUnit(int n) const override;
  const BYTE GetPlayer() const override;

  void Lock(class CCommonUnit *pUnit);
  bool IsLocked(int nPlayer) const;
  void Unlock(class CCommonUnit *pUnit);

  void Alarm();

  const int GetNGunsInFireSlot(int nSlot);
  CBasicGun *GetGunInFireSlot(int nSlot, int nGun);
  CTurret *GetTurretInFireSlot(int nSlot);
  float GetMaxFireRangeInSlot(int nSlot) const;

  bool IsSoldierVisible(int nParty, const CVec2 &center, bool bCamouflated, float fCamouflage) const;

  bool IsSelectable() const override;
  const bool IsVisibleForDiplomacyUpdate() override;

  bool CanUnitGoThrough(const EAIClass &eClass) const override;

  // Is it possible to change the slot for this slot?
  virtual bool CanRotateSoldier(class CSoldier *pSoldier) const;
  // put the soldier in place instead of the one sitting there
  virtual void ExchangeUnitToFireplace(class CSoldier *pSoldier, int nFirePlace);
  // number of fireplaces
  const int GetNFirePlaces() const;
  // a soldier sitting in a fireplace, if the fireplace is empty then returns 0
  class CSoldier *GetSoldierInFireplace(int nFireplace) const;

  void SetScriptID(const int _nScriptID) override { nScriptID = _nScriptID; }

  const NTimer::STime &GetLastLeaveTime(const int nPlayer) const { return lastLeave[nPlayer]; }
  void SetLastLeaveTime(int nPlayer);
};

// simple building
class CBuildingSimple : public CBuilding
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CBuildingSimple);

public:
  CBuildingSimple() {}

  CBuildingSimple(const SBuildingRPGStats *pStats, const CVec2 &center, const int dbID, const float fHP, const int nFrameIndex)
    : CBuilding(pStats, center, dbID, fHP, nFrameIndex) {}
};

// building that is a warehouse (main or intermediate)
class CBuildingStorage : public CBuilding
{
  DECLARE_SERIALIZE;
  OBJECT_COMPLETE_METHODS(CBuildingStorage);

  int nPlayer;// who owns this building
  bool bConnected;
  NTimer::STime timeLastBuildingRepair;// for self-medication of the main storage
protected:
  void AddSoldier(CSoldier *pUnit) override;

public:
  CBuildingStorage() : bConnected(true), timeLastBuildingRepair(0) {}
  CBuildingStorage(const SBuildingRPGStats *pStats, const CVec2 &center, int dbID, float fHP, int nFrameIndex, int player);

  void TakeDamage(float fDamage, bool bFromExplosion, int nPlayerOfShoot, CAIUnit *pShotUnit) override;

  const BYTE GetPlayer() const override;
  void SetPlayerForEditor(const int _nPlayer) override { nPlayer = _nPlayer; }

  void Segment() override;
  void SetHitPoints(float fNewHP) override;

  void SetConnected(bool bConnected);
  bool IsConnected() const { return bConnected; }
  void ChangePlayer(int nPlayer);

  void GetNewUnitInfo(SNewUnitInfo *pNewUnitInfo) override;

  bool ShouldSuspendAction(const EActionNotify &eAction) const override;
};

#endif // __BUILDING_H__