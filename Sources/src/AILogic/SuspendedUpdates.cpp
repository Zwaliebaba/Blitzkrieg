#include "stdafx.h"

#include "SuspendedUpdates.h"
#include "Diplomacy.h"
#include "RectTiles.h"
#include "LinkObject.h"
#include "Updater.h"
#include "Graveyard.h"

#include "../StreamIO/StreamIOTypes.h"
#include "../StreamIO/Globals.h"

// for debug
#include "MPLog.h"

CSuspendedUpdates theSuspendedUpdates;
extern CDiplomacy theDipl;
extern CUpdater updater;

// numbering - by update, which can be postponed, its serial number
std::unordered_map<int, int> numeration;

constexpr int N_CELL_SIZE = 8;
constexpr int N_SUSPENDED_ACTIONS = 7;

CSuspendedUpdates::CSuspendedUpdates()
  : objectsByCells(N_CELL_SIZE) {}

void CSuspendedUpdates::CommonInit()
{
  recalledUpdates.clear();
  recalledUpdates.resize(N_SUSPENDED_ACTIONS);
  numeration.clear();
  numeration.insert(std::pair<int, int>(ACTION_NOTIFY_DEAD_UNIT, 0));
  numeration.insert(std::pair<int, int>(ACTION_NOTIFY_GET_DEAD_UNITS_UPDATE, 1));
  numeration.insert(std::pair<int, int>(ACTION_NOTIFY_UPDATE_DIPLOMACY, 2));
  numeration.insert(std::pair<int, int>(ACTION_NOTIFY_RPG_CHANGED, 3));
  numeration.insert(std::pair<int, int>(ACTION_NOTIFY_NEW_ST_OBJ, 4));
  numeration.insert(std::pair<int, int>(ACTION_NOTIFY_SILENT_DEATH, 5));
  numeration.insert(std::pair<int, int>(ACTION_NOTIFY_CHANGE_FRAME_INDEX, 6));

  NI_ASSERT_T(numeration.size() == N_SUSPENDED_ACTIONS, "Wrong number of suspeneded actions");
}

void CSuspendedUpdates::Init(const int nStaticMapSizeX, const int nStaticMapSizeY)
{
  nMyParty = theDipl.GetMyParty();
  objectsByCells.SetSizes(nStaticMapSizeX / N_CELL_SIZE, nStaticMapSizeY / N_CELL_SIZE);

  CommonInit();
}

void CSuspendedUpdates::Clear()
{
  objectsByCells.Clear();

  updates.clear();
  tilesOfObj.clear();
  diplomacyUpdates.clear();
  recalledUpdates.clear();
  visibleTiles.clear();
}

void CSuspendedUpdates::TileBecameVisible(const SVector &tile, const int nParty)
{
  // we saw the tile and there are objects with suspended updates there
  if (nParty == nMyParty && !objectsByCells[tile.y / N_CELL_SIZE][tile.x / N_CELL_SIZE].empty()) visibleTiles.insert(tile);
}

void CSuspendedUpdates::DeleteUpdate(IUpdatableObj *pObj, const EActionNotify &eAction)
{
  // special case - delayed update diplomacy
  if (eAction == ACTION_NOTIFY_UPDATE_DIPLOMACY) diplomacyUpdates.erase(pObj);
  else
  {
    if (tilesOfObj.find(pObj->GetUniqueId()) != tilesOfObj.end())
    {
      const int nNumeration = numeration[eAction];
      if (updates[pObj].size() > nNumeration) updates[pObj][nNumeration] = nullptr;

      // remove from recalled updates
      CRecalledUpdatesType::iterator iter = recalledUpdates[nNumeration].begin();
      while (iter != recalledUpdates[nNumeration].end())
      {
        if (iter->pObj == pObj) iter = recalledUpdates[nNumeration].erase(iter);
        else ++iter;
      }

      // delete an object if this was its last update
      int i = 0;
      while (i < updates[pObj].size() && updates[pObj][i] == nullptr) ++i;

      if (i >= updates[pObj].size()) DeleteObjectInfo(pObj);
    }
  }
}

void CSuspendedUpdates::SuspendUpdate(const EActionNotify &eAction, IUpdatableObj *pObj, const SSuspendedUpdate &update)
{
  bool bShouldSuspend = true;
  // there is no such object yet
  if (tilesOfObj.find(pObj->GetUniqueId()) == tilesOfObj.end() || updates.find(pObj) == updates.end())
  {
    CTilesSet tiles;
    pObj->GetTilesForVisibility(&tiles);
    if (!tiles.empty())
    {
      updates[pObj].clear();
      updates[pObj].resize(N_SUSPENDED_ACTIONS);

      tilesOfObj[pObj->GetUniqueId()].clear();
      for (CTilesSet::const_iterator iter = tiles.begin(); iter != tiles.end(); ++iter)
      {
        tilesOfObj[pObj->GetUniqueId()].insert(*iter);
        objectsByCells.AddToPosition(pObj, *iter);
      }
    }
    else bShouldSuspend = false;
  }

  if (bShouldSuspend)
  {
    NI_ASSERT_T(numeration.find( eAction ) != numeration.end(), "Wrong action to suspend");
    NI_ASSERT_T(updates.find( pObj ) != updates.end(), "Updates of object haven't been initialized");
    updates[pObj][numeration[eAction]] = CreateObject<IDataStream>(STREAMIO_MEMORY_STREAM);
    update.Pack(updates[pObj][numeration[eAction]]);
  }
}

void CSuspendedUpdates::AddComplexObjectUpdate(const EActionNotify &eAction, IUpdatableObj *pObj, const SSuspendedUpdate &update) { SuspendUpdate(eAction, pObj, update); }

bool CSuspendedUpdates::CheckToSuspend(const EActionNotify &eAction, IUpdatableObj *pObj, const SSuspendedUpdate &update)
{
  if (pObj->ShouldSuspendAction(eAction))
  {
    // special case - delayed update diplomacy
    if (eAction == ACTION_NOTIFY_UPDATE_DIPLOMACY)
    {
      if (!pObj->IsVisibleForDiplomacyUpdate())
      {
        diplomacyUpdates[pObj] = CreateObject<IDataStream>(STREAMIO_MEMORY_STREAM);
        update.Pack(diplomacyUpdates[pObj]);

        return true;
      }
      DeleteUpdate(pObj, eAction);
      return false;
    }
    if (pObj->IsVisibleByPlayer() || !pObj->IsFree())
    {
      DeleteUpdate(pObj, eAction);
      return false;
    }
    SuspendUpdate(eAction, pObj, update);
    return true;
  }
  return false;
}

bool CSuspendedUpdates::IsRecalledEmpty(const EActionNotify &eAction) const { return numeration.find(eAction) == numeration.end() || recalledUpdates[numeration[eAction]].empty(); }

const int CSuspendedUpdates::GetNRecalled(const EActionNotify &eAction) const
{
  if (numeration.find(eAction) == numeration.end()) return 0;
  const int nNumeration = numeration[eAction];
  return recalledUpdates[nNumeration].size();
}

void CSuspendedUpdates::GetRecalled(const EActionNotify &eAction, SSuspendedUpdate *pUpdate)
{
  NI_ASSERT_T(numeration.find( eAction ) != numeration.end(), NStr::Format( "Can't recall %d action", eAction ));

  const int nNumeration = numeration[eAction];
  NI_ASSERT_T(!recalledUpdates[nNumeration].empty(), "Recalled updates is already empty");

  recalledUpdates[nNumeration].back().Recall(pUpdate);
  recalledUpdates[nNumeration].pop_back();
}

void CSuspendedUpdates::UpdateVisibleTiles(const std::unordered_set<SVector, STilesHash> &tilesSet, std::unordered_set<SVector, STilesHash> *pCoverTiles)
{
  for (auto visTilesIter = tilesSet.begin(); visTilesIter != visibleTiles.end(); ++visTilesIter)
  {
    const SVector tile = *visTilesIter;
    const int nCellX = tile.x / N_CELL_SIZE;
    const int nCellY = tile.y / N_CELL_SIZE;

    auto objIter = objectsByCells[nCellY][nCellX].begin();
    while (objIter != objectsByCells[nCellY][nCellX].end())
    {
      IUpdatableObj *pObj = *objIter;
      ++objIter;
      // the object became visible
      const int nUniqueId = pObj->GetUniqueId();
      if (tilesOfObj[nUniqueId].find(tile) != tilesOfObj[nUniqueId].end())
      {
        if (updates.find(pObj) != updates.end()) { for (int i = 0; i < recalledUpdates.size(); ++i) { if (updates[pObj][i] != nullptr) recalledUpdates[i].push_back(SRecalledUpdate(pObj, updates[pObj][i])); } }

        if (pCoverTiles) pCoverTiles->insert(tilesOfObj[nUniqueId].begin(), tilesOfObj[nUniqueId].end());

        DeleteObjectInfo(pObj);
      }
    }
  }
}

void CSuspendedUpdates::Segment()
{
  std::unordered_set<SVector, STilesHash> coveredTiles;
  UpdateVisibleTiles(visibleTiles, &coveredTiles);
  visibleTiles.clear();
  UpdateVisibleTiles(coveredTiles, nullptr);

  std::list<IUpdatableObj *> delDiplomacyUpdates;
  for (auto iter = diplomacyUpdates.begin(); iter != diplomacyUpdates.end(); ++iter)
  {
    if (iter->first->IsVisibleForDiplomacyUpdate())
    {
      recalledUpdates[numeration[ACTION_NOTIFY_UPDATE_DIPLOMACY]].push_back(SRecalledUpdate(iter->first, iter->second));
      delDiplomacyUpdates.push_back(iter->first);
    }
  }

  for (auto iter = delDiplomacyUpdates.begin(); iter != delDiplomacyUpdates.end(); ++iter) diplomacyUpdates.erase(*iter);
}

void CSuspendedUpdates::DeleteObjectInfo(IUpdatableObj *pObj)
{
  // remove object from cells
  for (std::unordered_set<SVector, STilesHash>::iterator tilesOfObjIter = tilesOfObj[pObj->GetUniqueId()].begin(); tilesOfObjIter != tilesOfObj[pObj->GetUniqueId()].end(); ++tilesOfObjIter) objectsByCells.RemoveFromPosition(pObj, *tilesOfObjIter);

  // remove all tiles for object visibility
  tilesOfObj.erase(pObj->GetUniqueId());
  // remove all object updates
  updates.erase(pObj);
}

void CSuspendedUpdates::DeleteUpdates(IUpdatableObj *pObj) { if (tilesOfObj.find(pObj->GetUniqueId()) != tilesOfObj.end()) DeleteObjectInfo(pObj); }

bool CSuspendedUpdates::DoesExistSuspendedUpdate(IUpdatableObj *pObj, const EActionNotify &eAction) { return updates.find(pObj) != updates.end() && updates[pObj][numeration[eAction]] != nullptr; }

void CSuspendedUpdates::SRecalledUpdate::Recall(SSuspendedUpdate *pRecallTo)
{
  if (pUpdateInfo)
  {
    pUpdateInfo->Seek(0, STREAM_SEEK_SET);
    pRecallTo->Recall(pUpdateInfo);
  }
  pRecallTo->pObj = pObj;

  updater.Add2Garbage(pObj);
}