#ifndef __UPDATER_H__
#define __UPDATER_H__

#pragma once

#include "../Common/Actions.h"
#include "AIHashFuncs.h"

interface IUpdatableObj;

class CUpdater
{
  // OBJECT_NORMAL_METHODS( CUpdater );
  DECLARE_SERIALIZE;

  const static int nUpdateTypes;

  CPtr<ISegmentTimer> pGameSegment;
  CPtr<IGameTimer> pGameTimer;

public:
  struct SSimpleUpdate
  {
    CObj<IUpdatableObj> pObj;
    int nParam;

    SSimpleUpdate() : nParam(-1) {}
    SSimpleUpdate(IUpdatableObj *_pObj, const int _nParam) : pObj(_pObj), nParam(_nParam) {}
    SSimpleUpdate(const SSimpleUpdate &update) : pObj(update.pObj), nParam(update.nParam) {}

    bool operator==(const SSimpleUpdate &simpleUpdate) const { return pObj == simpleUpdate.pObj; }
    // const int GetUniqueId() const;

    virtual int STDCALL operator&(interface IStructureSaver &ss)
    {
      CSaverAccessor saver = &ss;
      saver.Add(1, &pObj);
      saver.Add(2, &nParam);
      return 0;
    }
  };

private:
  using CAnimationSet = std::unordered_map<int, int>;

  using CSimpleUpdatesSet = std::unordered_map<int, SSimpleUpdate>;
  using CComplexUpdatesSet = std::unordered_map<int, CObj<IUpdatableObj>>;

  // simpleUpdate - those actions that have a 1 at the end, complexUpdates - those actions that have a 0 at the end
  std::vector<CSimpleUpdatesSet> simpleUpdates;
  std::vector<CComplexUpdatesSet> complexUpdates;

  std::list<SAIFeedBack> feedBacks;
  CAnimationSet unitAnimation;

  CComplexUpdatesSet garbage;
  CComplexUpdatesSet updatedPlacements;

  // to delete updates placement only after they came to Yura
  bool bPlacementsUpdated;
  bool bDestroying;

  int nShootAreasGroup;
  bool bGameFinishUpdateSend;
  //
  void DestroyContents();
  // depending on the type of update, adds it to the desired array - simpleUpdates or complexUpdates
  void AddUpdate(EActionNotify updateType, IUpdatableObj *pObj, int nParam);

public:
  CUpdater();
  ~CUpdater();

  void Init();
  void Clear() { DestroyContents(); }

  void EndUpdates();

  void Update(enum EActionNotify updateType, IUpdatableObj *pObj, int nParam = -1);
  // for an object that consists of several parts.
  // void UpdateComplexObject( const EActionNotify eAction, IUpdatableObj * pObj );
  void DelUpdate(enum EActionNotify updateType, IUpdatableObj *pObj);
  void DelActionUpdates(IUpdatableObj *pObj);
  void ClearAllUpdates(enum EActionNotify updateType);
  void ClearAllUpdates();

  void UpdateActions(struct SAINotifyAction **pActionsBuffer, int *pnLen);

  void UpdatePlacements(struct SAINotifyPlacement **pObjPosBuffer, int *pnLen);
  void UpdateRPGParams(struct SAINotifyRPGStats **pUnitRPGBuffer, int *pnLen);
  void UpdateShots(struct SAINotifyMechShot **pShots, int *pnLen);
  void UpdateShots(struct SAINotifyInfantryShot **pShots, int *pnLen);
  void UpdateHits(struct SAINotifyHitInfo **pHits, int *pnLen);
  void UpdateStObjPlacements(struct SAINotifyPlacement **pObjPosBuffer, int *pnLen);
  void UpdateTurretTurn(struct SAINotifyTurretTurn **pTurretsBuffer, int *pnLen);
  void UpdateEntranceStates(SAINotifyEntranceState **pUnits, int *pnLen);
  void UpdateDiplomacies(struct SAINotifyDiplomacy **pDiplomaciesBuffer, int *pnLen);
  void UpdateShootAreas(struct SShootAreas **pShootAreas, int *pnLen);
  void UpdateRangeAreas(struct SShootAreas **pRangeAreas, int *pnLen);

  void GetNewProjectiles(struct SAINotifyNewProjectile **pProjectiles, int *pnLen);
  void GetDeadProjectiles(IRefCount ***pProjectilesBuf, int *pnLen);

  void GetNewUnits(struct SNewUnitInfo **pNewUnitBuffer, int *pnLen);
  void GetDisappearedUnits(IRefCount ***pUnitsBuffer, int *pnLen);

  void GetNewStaticObjects(struct SNewUnitInfo **pObjects, int *pnLen);
  void GetDeletedStaticObjects(IRefCount ***pObjBuffer, int *pnLen);
  void GetEntrenchments(struct SSegment2Trench **pEntrenchemnts, int *pnLen);
  void GetFormations(struct SSoldier2Formation **pFormations, int *pnLen);
  void GetNewBridgeSpans(struct SNewUnitInfo **pObjects, int *pnLen);
  void GetRevealCircles(CCircle **pCircleBuffer, int *pnLen);

  //
  void AddFeedBack(const SAIFeedBack &feedBack);
  void UpdateFeedBacks(struct SAIFeedBack **pFeedBacksBuffer, int *pnLen);

  //
  void UpdateAreasGroup(int nGroup);
  void ResetAreasGroupIfEqual(const int nGroup) { if (nShootAreasGroup == nGroup) UpdateAreasGroup(-1); }

  bool IsPlacementUpdated(IUpdatableObj *pObj) const;
  void ClearPlacementsUpdates();

  void Add2Garbage(IUpdatableObj *pObj);
};

inline const NTimer::STime GetAIGetSegmTime(ISegmentTimer *pGameSegment) { return pGameSegment->Get() + pGameSegment->GetSegmentTime(); }

#endif // __UPDATER_H__