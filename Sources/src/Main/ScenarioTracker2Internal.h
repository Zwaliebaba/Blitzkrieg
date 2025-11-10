#ifndef __SCENARIOTRACKER2INTERNAL_H__
#define __SCENARIOTRACKER2INTERNAL_H__

#pragma once

#include "ScenarioTracker.h"
#include "PlayerScenarioInfo.h"

using CPlayerScenarioInfoPair = std::pair<int, CObj<CPlayerScenarioInfo>>;
using CPlayersList = std::list<CPlayerScenarioInfoPair>;

// ************************************************************************************************************************ //
// **
// ** player scenario info iterator
// **
// **
// **
// ************************************************************************************************************************ //

class CPlayerScenarioInfoIterator : public CTRefCount<IPlayerScenarioInfoIterator>
{
  const CPlayersList &players;
  CPlayersList::const_iterator itCurrPlayer;

public:
  CPlayerScenarioInfoIterator(const CPlayersList &players);
  // move to next player scenario info
  void STDCALL Next() override;
  // check, if we've reached end?
  bool STDCALL IsEnd() const override;
  // get current iterator's player scenario info
  IPlayerScenarioInfo * STDCALL Get() const override;
  // get current iterator's player ID
  int STDCALL GetID() const override;
};

// ************************************************************************************************************************ //
// **
// ** scenario tracker
// **
// **
// **
// ************************************************************************************************************************ //

class CScenarioTracker2 : public CTRefCount<IScenarioTracker>
{
  OBJECT_SERVICE_METHODS(CScenarioTracker2);
  DECLARE_SERIALIZE;

  // names usage statistics
  struct SNameUsageStats
  {
    std::string szName;// name file name
    NTimer::STime timeLastUsage;// time of last use of this name
    int nUsedCounter;// how many times has this name been used?
    int nUsage;// how much is it used now?
    //
    SNameUsageStats()
      : timeLastUsage(0), nUsedCounter(0), nUsage(0) {}

    //
    SNameUsageStats &operator=(const SNameUsageStats &stats)
    {
      szName = stats.szName;
      timeLastUsage = stats.timeLastUsage;
      nUsedCounter = stats.nUsedCounter;
      nUsage = stats.nUsage;
      return *this;
    }

    //
    bool operator<(const SNameUsageStats &stats) const
    {
      if (timeLastUsage == stats.timeLastUsage) return nUsedCounter == stats.nUsedCounter ? nUsage < stats.nUsage : nUsedCounter < stats.nUsedCounter;
      return timeLastUsage < stats.timeLastUsage;
    }

    //
    int operator&(IStructureSaver &ss)
    {
      CSaverAccessor saver = &ss;
      saver.Add(1, &szName);
      saver.Add(2, &timeLastUsage);
      saver.Add(3, &nUsedCounter);
      saver.Add(4, &nUsage);
      return 0;
    }
  };

  // oponent description
  struct SOpponentDesc
  {
    struct SRPGClassDesc
    {
      std::string szClassName;// RPG class name
      enum EUnitRPGClass eRPGClass;// RPG class constant
      std::vector<std::string> names;// files with personal names
      //
      int operator&(IDataTree &ss);
    };

    //
    std::string szSide;// side: USSR, German, Allies, etc.
    std::vector<SRPGClassDesc> classes;// RPG classes descriptions
    //
    int operator&(IDataTree &ss);
  };

  // player
  CPlayersList players;// all players in the scenario tracker
  CPtr<CPlayerScenarioInfo> pUserPlayer;// user player
  int nUserPlayerID;// user player ID
  std::string szMinimumDifficulty;
  //
  mutable std::vector<SOpponentDesc> opponents;// opponents descriptions
  using CNamesUsageMap = std::hash_map<std::string, SNameUsageStats>;
  CNamesUsageMap personalNamesUsage;// personal names usage
  // current chapter script
  class Script *pChapterScript;// chapter script
  std::string szChapterScriptFileName;// current script file name
  // campaign, chapter, mission
  std::string szCurrCampaign;// currently started campaign
  std::string szCurrChapter;// current chapter name
  std::string szCurrMission;// current mission name
  ECampaignType eCampaignType;// campaign's type
  std::vector<std::string> templateMissions;// all available template missions
  std::vector<std::list<std::string>> randomBonuses;
  // current mission GUID
  GUID guidMission;//
  //
  std::string GetBestPersonalName(const std::string &szRPGStats, const std::string &szSide) const;
  void AssignBestPersonalName(CScenarioUnit *pUnit, const std::string &szSide);
  bool LoadChapterScript(const std::string &szScriptFileName);
  void ProcessScriptChanges(bool bPostMission);
  void LoadOpponents() const;
  void InitMinimumDifficulty();

public:
  CScenarioTracker2();
  //
  bool STDCALL Init(ISingleton *pSingleton) override;
  //
  // players
  //
  // add new player with 'nPlayerID'. 
  IPlayerScenarioInfo * STDCALL AddPlayer(int nPlayerID) override;
  // remove player with 'nPlayerID'
  bool STDCALL RemovePlayer(int nPlayerID) override;
  // get player with 'nPlayerID'.
  IPlayerScenarioInfo * STDCALL GetPlayer(int nPlayerID) const override;
  // set user player
  void STDCALL SetUserPlayer(int nPlayerID) override;
  // get user player interface
  IPlayerScenarioInfo * STDCALL GetUserPlayer() const override;
  int STDCALL GetUserPlayerID() const override;
  // iterate through all players
  IPlayerScenarioInfoIterator * STDCALL CreatePlayerScenarioInfoIterator() const override;
  //
  // campaign, chapters, missions
  //
  // start new campaign (for all players)
  void STDCALL StartCampaign(const std::string &szCampaignName, ECampaignType eType) override;
  // start new chapter (and finish previous one)
  bool STDCALL StartChapter(const std::string &szChapterName) override;
  // start new mission
  void STDCALL StartMission(const std::string &szMissionName) override;
  // finish mission
  void STDCALL FinishMission(EMissionFinishStatus eStatus) override;
  // current mission GUID
  const GUID & STDCALL GetCurrMissionGUID() const override { return guidMission; }
  // 
  // minimum difficulty tracking
  //
  void STDCALL UpdateMinimumDifficulty() override;
  const std::string & STDCALL GetMinimumDifficulty() const override;
  //
  // campaign random mission templates
  //
  // number of available random mission templates
  int STDCALL GetNumRandomTemplates() const override;
  // get template by number
  const std::string & STDCALL GetTemplateName(int nIndex) const override;

  // removes all random bonuses
  void STDCALL ClearRandomBonuses(int nDifficulty) override;
  // adds a random bonus
  bool STDCALL AddRandomBonus(int nDifficulty, const std::string &rszRandomBonus) override;
  // returns the random bonus, removing it from the list
  std::string STDCALL GetRandomBonus(int nDifficulty) override;

  //
  IScenarioTracker * STDCALL Duplicate() const override;
  int STDCALL operator&(IDataTree &ss) override;
};

#endif // __SCENARIOTRACKER2INTERNAL_H__