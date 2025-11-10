#ifndef __MULTIPLAYER_INFO_H__
#define __MULTIPLAYER_INFO_H__

#pragma once

#include "../RandomMapGen/MapInfo_Types.h"

class CMultiplayerInfo
{
  enum CMapInfo::GAME_TYPE eGameType;

  struct SWinConditions
  {
    // FC settings
    int nFlagScoreLimit;
    int nKillScoreLimit;
    //

    // sabotage settings
    NTimer::STime nTimeToCapture;// in seconds

    // common settings
    NTimer::STime nTimeLimit;// in seconds
  };

  SWinConditions winConditions;

  bool bNoWin;

  std::vector<float> flagPoints;
  std::vector<float> flagPoints4Script;
  std::vector<float> troopsPoints;

  // for sabotage mission
  NTimer::STime timeOfFlagCaptured;
  bool bCapturedByAttackingParty;
  int nAttackingParty;
  int nFlagsAtTheMap;

  std::vector<std::hash_set<int>> capturedByPartyFlags;

  //
  void CheckWinConditions();
  void GameFinished(EFeedBack eGameResult);

  void CheckSabotageWinConditions();
  void CheckFlagControlWinConditions();
  void CheckFlagPoints();
  void CheckTroopPoints();

public:
  CMultiplayerInfo()
    : bNoWin(false), flagPoints(3, 0.0f), flagPoints4Script(3, 0.0f), troopsPoints(3, 0.0f), nFlagsAtTheMap(0) {}

  void Clear();

  void Init();
  void Segment();

  void AddFlagPoints(int nParty, float fPoints);
  void FlagCaptured(int nParty, int nFlagID);
  void UnitsKilled(int nKillerPlayer, float fUnitsPrice, int nKilledUnitsPlayer);

  // the time required for a flag within which only one player's units are in radius to pass to that player
  const NTimer::STime GetTimeToCaptureObject() const;

  void AddFlagAtTheMap() { ++nFlagsAtTheMap; }

  bool IsSabotage() const { return eGameType == CMapInfo::TYPE_SABOTAGE; }
  const int GetAttackingParty() const { return nAttackingParty; }

  void NoWin();
  bool IsNoWin() const { return bNoWin; }
};

#endif // __MULTIPLAYER_INFO_H__