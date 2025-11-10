#ifndef __PLAYERLISTMANAGER_H__
#define __PLAYERLISTMANAGER_H__

#pragma once

#include "MultiplayerCommandManager.h"
#include "InterMission.h"
#include "iMission.h"
#include "../Misc/FreeIDs.h"

// for convinient management of list control
template<class TInfo, class TID>
class CListControlWrapper
{
  CFreeIds items;

  CPtr<IUIListControl> pPlayersList;// list control shortcut

  using IDToPosMap = std::hash_map<TID, int>;
  using PosToIDMap = std::hash_map<int, TID>;


  IDToPosMap playerIDToPos;
  PosToIDMap posToPlayerID;
  //

  using PlayersInfo = std::hash_map<TID, CPtr<TInfo>>;
  PlayersInfo playersInfo;

  PlayersInfo::iterator curIter;// current iterator.


public:
  CListControlWrapper() {}
  void ResetIterator() { curIter = playersInfo.begin(); }
  unsigned int GetSize() const { return playersInfo.size(); }

  SUIPlayerInfo *GetNext()
  {
    if (curIter == playersInfo.end()) return nullptr;
    return (curIter++)->second;
  }

  void SelectFirst() { pPlayersList->SetSelectionItem(0); }

  void Clear()
  {
    while (pPlayersList->GetNumberOfItems()) { pPlayersList->RemoveItem(0); }
    playerIDToPos.clear();
    posToPlayerID.clear();
    playersInfo.clear();
  }

  void SetListControl(IUIListControl *pListCtrl)
  {
    pPlayersList = pListCtrl;
    NI_ASSERT(pPlayersList != 0);
    pPlayersList->InitialUpdate();
  }

  TInfo *GetCurInfo()
  {
    const int nIndex = pPlayersList->GetSelectionItem();
    if (-1 != nIndex)
    {
      IUIListRow *pRow = pPlayersList->GetItem(nIndex);
      if (pRow)
      {
        const int nUserData = pRow->GetUserData();
        if (posToPlayerID.find(nUserData) == posToPlayerID.end() ||
            playersInfo.find(posToPlayerID[nUserData]) == playersInfo.end())
          return nullptr;
        return playersInfo[posToPlayerID[nUserData]];
      }
    }
    return nullptr;
  }

  IUIListRow *Add(TInfo *pInfo)
  {
    /* int nCount = playersInfo.size();
     */

    const TID nID = pInfo->GetID();
    PlayersInfo::iterator playerIter = playersInfo.find(nID);
    if (playerIter == playersInfo.end())
    {
      // insert player
      const int nPos = items.GetFreeId();
      pPlayersList->AddItem(nPos);
      posToPlayerID[nPos] = nID;
      playerIDToPos[nID] = nPos;
      playersInfo[nID] = pInfo;
      playerIter = playersInfo.find(nID);
      pPlayersList->InitialUpdate();
    }
    else { playerIter->second = pInfo; }
    NI_ASSERT_T(playerIter != playersInfo.end(), "something wron");
    const int nPos = playerIDToPos[nID];
    const int nIndex = pPlayersList->GetItemByID(nPos);
    if (-1 != nIndex) return pPlayersList->GetItem(nIndex);
    return nullptr;
  }

  void Delete(const TID &nID)
  {
    if (playerIDToPos.find(nID) == playerIDToPos.end()) return;

    const int nPos = playerIDToPos[nID];

    playerIDToPos.erase(nID);
    playersInfo.erase(nID);
    posToPlayerID.erase(nPos);

    const int nIndex = pPlayersList->GetItemByID(nPos);
    pPlayersList->RemoveItem(nIndex);
    items.AddToFreeId(nPos);
    pPlayersList->InitialUpdate();
  }

  void Resort() { pPlayersList->ReSort(); }

  void Delete(const TInfo *pInfo)
  {
    // remove item from list
    const TID nID = pInfo->GetID();
    Delete(nID);
  }

  bool IsEmpty() const { return playersInfo.empty(); }

};

#endif // __PLAYERLISTMANAGER_H__