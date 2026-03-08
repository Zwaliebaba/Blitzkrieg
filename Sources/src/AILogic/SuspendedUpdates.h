#ifndef __SUSPENDED_UPDATES_H__
#define __SUSPENDED_UPDATES_H__

#pragma once

#include "../Misc/AreaMap.h"
#include "UpdatableObject.h"
#include "AIHashFuncs.h"

class CSuspendedUpdates
{
  DECLARE_SERIALIZE;

  // deferred updates
  // by cell - list of objects that store updates in it
  using CObjectsByCells = CAreaMap<IUpdatableObj, CPtr<IUpdatableObj>, SVector, int>;
  CObjectsByCells objectsByCells;
  // by object - updates that are postponed for it
  std::unordered_map<CObj<IUpdatableObj>, std::vector<CPtr<IDataStream>>, SUpdatableObjectObjHash> updates;
  // by object - tiles, when visible, all pending updates should be sent
  using CTilesMap = std::unordered_map<int, std::unordered_set<SVector, STilesHash>>;
  CTilesMap tilesOfObj;

  // object - deferred diplomacy updates
  // It is expected that there will be few such updates
  using CDiplomacyUpdatesType = std::unordered_map<CObj<IUpdatableObj>, CPtr<IDataStream>, SUpdatableObjectObjHash>;
  CDiplomacyUpdatesType diplomacyUpdates;

  // for each eAction all recalled updates
  struct SRecalledUpdate
  {
    virtual int STDCALL operator&(IStructureSaver &ss)
    {
      CSaverAccessor saver = &ss;
      saver.Add(1, &pObj);
      saver.Add(3, &pUpdateInfo);
      return 0;
    }

    CObj<IUpdatableObj> pObj;
    CPtr<IDataStream> pUpdateInfo;

    SRecalledUpdate() {}

    SRecalledUpdate(IUpdatableObj *_pObj, IDataStream *_pUpdateInfo)
      : pObj(_pObj), pUpdateInfo(_pUpdateInfo) {}

    void Recall(SSuspendedUpdate *pRecallTo);
  };

  using CRecalledUpdatesType = std::list<SRecalledUpdate>;
  std::vector<CRecalledUpdatesType> recalledUpdates;

  // tiles that became visible after the next scan of the fog
  std::unordered_set<SVector, STilesHash> visibleTiles;

  int nMyParty;

  //
  // delete everything related to pObj, except diplomacy updates
  void DeleteObjectInfo(IUpdatableObj *pObj);
  // removes pending updates for eAction object pObj
  void DeleteUpdate(IUpdatableObj *pObj, const EActionNotify &eAction);
  void CommonInit();
  void SuspendUpdate(const EActionNotify &eAction, IUpdatableObj *pObj, const SSuspendedUpdate &update);
  void UpdateVisibleTiles(const std::unordered_set<SVector, STilesHash> &tilesSet, std::unordered_set<SVector, STilesHash> *pCoverTiles);

public:
  CSuspendedUpdates();

  void Init(int nStaticMapSizeX, int nStaticMapSizeY);
  void Clear();

  // composite object update
  void AddComplexObjectUpdate(const EActionNotify &eAction, IUpdatableObj *pObj, const SSuspendedUpdate &update);

  // called when scanning warfog, when the tile has become visible to the nParty side
  void TileBecameVisible(const SVector &tile, int nParty);

  // check whether it needs to be postponed; 
  bool CheckToSuspend(const EActionNotify &eAction, IUpdatableObj *pObj, const SSuspendedUpdate &update);

  // there is a deferred update eAction for pObj
  bool DoesExistSuspendedUpdate(IUpdatableObj *pObj, const EActionNotify &eAction);

  // Are there any thoughtful updates left for this eActions?
  bool IsRecalledEmpty(const EActionNotify &eAction) const;
  const int GetNRecalled(const EActionNotify &eAction) const;

  // record the next remembered update for eAction
  void GetRecalled(const EActionNotify &eAction, SSuspendedUpdate *pUpdate);
  // delete everything related to pObj if there are any updates for pObj other than diplomacy updates
  void DeleteUpdates(IUpdatableObj *pObj);

  void Segment();
};

#endif // __SUSPENDED_UPDATES_H__