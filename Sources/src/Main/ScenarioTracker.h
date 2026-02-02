#ifndef __SCENARIOTRACKER2_H__
#define __SCENARIOTRACKER2_H__

#pragma once

#include "iMainClassIDs.h"

#define NUM_MEDAL_SLOTS 6

// ************************************************************************************************************************ //
// **
// ** some defs
// **
// **
// **
// ************************************************************************************************************************ //

enum EMissionFinishStatus
{
  MISSION_FINISH_UNKNOWN = -1,
  MISSION_FINISH_WIN = 0,
  MISSION_FINISH_LOSE = 1,
  MISSION_FINISH_ABORT = 2,
  MISSION_FINISH_RESTART = 3,

  MISSION_FINISH_FORCE_DWORD = 0x7fffffff
};

enum ECampaignType
{
  CAMPAIGN_TYPE_UNKNOWN = -1,
  CAMPAIGN_TYPE_SINGLE = 0,
  CAMPAIGN_TYPE_MULTIPLAYER = 1,
  CAMPAIGN_TYPE_CUSTOM_CAMPAIGN = 2,
  CAMPAIGN_TYPE_CUSTOM_CHAPTER = 3,
  CAMPAIGN_TYPE_CUSTOM_MISSION = 4,
  CAMPAIGN_TYPE_TUTORIAL = 5,

  CAMPAIGN_FORCE_DWORD = 0x7fffffff
};

enum EPlayerSkillType
{
  EPST_TACTICS = 0,
  EPST_LOGISTICS = 1,
  EPST_CAREFULNESS = 2,
  EPST_STAFF = 3,
  EPST_ARTOFWAR = 4,
  EPST_DUTY = 5,
  _EPST_COUNT = 6,
};

enum EStatsComplexity
{
  STATS_COMPLEXITY_TOTAL = 0,
  STATS_COMPLEXITY_CHAPTER = 1,
  STATS_COMPLEXITY_MISSION = 2,

  STATS_COMPLEXITY_FORCE_DWORD = 0x7fffffff
};

// ************************************************************************************************************************ //
// **
// ** current active user profile
// **
// ** * binds
// ** * options (name and MP name included)
// ** * cutscenes availability
// ** * help screens tracking
// ** * template usage statistics
// ** * personal names usage
// **
// **
// ************************************************************************************************************************ //

interface IUserProfile : IRefCount
{
  enum { tidTypeID = MAIN_USER_PROFILE };

  // help screens tracker part
  //
  virtual bool IsHelpCalled(int nInterfaceTypeID, int nHelpNumber) const = 0;
  virtual void HelpCalled(int nInterfaceTypeID, int nHelpNumber) = 0;
  //
  // cutscenes availability
  //
  virtual void AddCutScene(const std::string &szCutSceneName) = 0;
  virtual int GetNumCutScenes() const = 0;
  virtual const std::string & GetCutScene(int nIndex) const = 0;
  //
  // templates usage statistics
  //
  virtual void AddUsedTemplate(const std::string &rszTemplate, int nTemplateWeight, const std::string &rszGraph, int nGraphWeight, int nAngle, int nAngleWeight) = 0;
  virtual int GetUsedTemplates(const std::string &rszTemplate) = 0;
  virtual int GetUsedTemplateGraphs(const std::string &rszTemplate, const std::string &rszGraph) = 0;
  // 0...3
  virtual int GetUsedAngles(int nAngle) = 0;
  //
  // users relations for chat
  //
  virtual void SetChatRelation(const wchar_t *pwszNick, enum EPlayerRelation eRelation) = 0;
  virtual const enum EPlayerRelation GetChatRelation(const wchar_t *pwszNick) = 0;
  //
  // current MOD
  // gets and returns dirname (subdirectory in Mods directory)
  // returns with '\\' at the end.
  //
  virtual void SetMOD(const std::string &szMOD) = 0;
  virtual const std::string & GetMOD() const = 0;
  //
  // loads counters, based on GUID for each mission
  //
  virtual void RegisterLoad(const GUID &guid) = 0;
  virtual int GetLoadCounter(const GUID &guid) const = 0;

  //
  // user values ​​(stored in profile)
  //
  virtual void AddVar(const char *pszValueName, int nValue) = 0;
  virtual int GetVar(const char *pszValueName, int nDefValue) const = 0;
  virtual void RemoveVar(const char *pszValueName) = 0;

  //
  // serialization & repair
  //
  virtual bool IsChanged() const = 0;
  virtual void SerializeConfig(IDataTree *pSS) = 0;
  virtual void Repair(IDataTree *pSS, bool bToDefault) = 0;
};

// ************************************************************************************************************************ //
// **
// ** scenario unit stats
// **
// ** * unit values ​​(kills, exp, level, etc.)
// ** * personal name
// ** * RPG stats
// **
// ************************************************************************************************************************ //

interface IScenarioUnit : IRefCount
{
  // unit's values
  virtual void SetValue(int nType, int nValue) = 0;
  virtual void AddValue(int nType, int nValue) = 0;
  virtual int GetValue(int nType) const = 0;
  virtual int GetValueDiff(int nType) const = 0;
  // kill
  virtual void Kill() = 0;
  // personal name
  virtual interface IText * GetName() const = 0;
  // RPG stats
  virtual void ChangeRPGStats(const std::string &szStatsName) = 0;
  virtual const std::string & GetRPGStats() const = 0;
  // scenario ID
  virtual int GetScenarioID() const = 0;
};

// ************************************************************************************************************************ //
// **
// ** player scenario info (campaign-specific values)
// **
// ** * current player side (USSR, German, Allies, etc)
// ** * rank & skills
// ** * color on the map
// ** * units
// ** * medals
// ** * upgrade
// ** *depot
// **
// ************************************************************************************************************************ //

interface IPlayerScenarioInfo : IRefCount
{
  // player name
  virtual void SetName(const std::wstring &wszName) = 0;
  virtual const std::wstring & GetName() const = 0;
  virtual IText * GetNameObject() const = 0;
  //
  // current player side (USSR, German, Poland, French, GB, etc) and general side (USSR, German, Allies)
  //
  // side/general side
  virtual void SetSide(const std::string &szSideName) = 0;
  virtual const std::string & GetSide() const = 0;
  virtual const std::string & GetGeneralSide() const = 0;
  virtual interface IText * GetSideName() const = 0;
  // diplomacy side [0..2] (0 <=> 1 opponents, 2 - neutral)
  virtual void SetDiplomacySide(int nDiplomacySide) = 0;
  virtual const int GetDiplomacySide() const = 0;
  // player color
  virtual void SetColor(DWORD dwColor) = 0;
  virtual DWORD GetColor() const = 0;
  //
  // rank & skills
  //
  virtual const struct SPlayerSkill & GetSkill(int nSkill) const = 0;
  virtual void SetSkill(int nSkill, float fVal) = 0;
  // player's rank
  virtual const struct SPlayerRank & GetRankInfo() const = 0;
  virtual void ClearLevelGain() = 0;
  virtual bool IsGainLevel() const = 0;
  // set player's experience. 
  virtual bool SetExperience(double fExperience) = 0;
  //
  // units
  //
  virtual int GetNumUnits() const = 0;
  virtual IScenarioUnit * GetUnit(int nIndex) const = 0;
  virtual int GetNumNewUnits() const = 0;
  virtual IScenarioUnit * GetNewUnit(int nIndex) const = 0;
  //
  // medals
  //
  // get medal in purticular slot or return 0 if this slot are empty
  virtual const std::string & GetMedalInSlot(int nSlot) const = 0;
  virtual bool HasMedal(const std::string &szName) const = 0;
  // new medals
  virtual int GetNumNewMedals() const = 0;
  virtual const std::string & GetNewMedal(int nIndex) const = 0;
  //
  // upgrade and depot
  //
  // get available upgrade (can be empty)
  virtual const std::string & GetUpgrade() const = 0;
  // depot (unlimited) upgrades
  virtual int GetNumDepotUpgrades() const = 0;
  virtual const std::string & GetDepotUpgrade(int nIndex) const = 0;
  virtual void OrderDepotUpgrade(int nUpgradeIndex) = 0;
  virtual int GetNumNewDepotUpgrades() const = 0;
  virtual const std::string & GetNewDepotUpgrade(int nIndex) const = 0;
  virtual void ClearNewDepotUpgrade() = 0;
  //
  //
  //
  // get current campaign stats
  virtual interface ICampaignStatistics * GetCampaignStats() const = 0;
  // get current chapter stats
  virtual interface IChapterStatistics * GetChapterStats() const = 0;
  // get current mission stats
  virtual interface IMissionStatistics * GetMissionStats() const = 0;
};

// ************************************************************************************************************************ //
// **
// ** player scenario info iterator
// **
// **
// **
// ************************************************************************************************************************ //

interface IPlayerScenarioInfoIterator : IRefCount
{
  // move to next player scenario info
  virtual void Next() = 0;
  // check, if we've reached end?
  virtual bool IsEnd() const = 0;
  // get current iterator's player scenario info
  virtual IPlayerScenarioInfo * Get() const = 0;
  // get current iterator's player ID
  virtual int GetID() const = 0;
};

// ************************************************************************************************************************ //
// **
// ** mission/chapter/campaign statistics
// **
// **
// **
// ************************************************************************************************************************ //

interface IScenarioStatistics : IRefCount
{
  // get mission/chapter/campaign name
  virtual const std::string & GetName() const = 0;
  // retrieve statistics value by type
  virtual int GetValue(int nType) const = 0;
  // number of killed in action (KIA)
  virtual int GetNumKIA() const = 0;
  // KIA name (file name with localized name)
  virtual const std::string & GetKIAName(int nIndex) const = 0;
  // KIA new name (file name with localized name)
  virtual const std::string & GetKIANewName(int nIndex) const = 0;
  // KIA RPG stats
  virtual const std::string & GetKIAStats(int nIndex) const = 0;
};

interface IMissionStatistics : IScenarioStatistics
{
  // add (increment) value
  virtual void AddValue(int nType, int nValue) = 0;
  // set value directly (override)
  virtual void SetValue(int nType, int nValue) = 0;
  // get mission finish status
  virtual EMissionFinishStatus GetFinishStatus() const = 0;
};

interface IChapterStatistics : IScenarioStatistics
{
  // get num missions, ever started in this chapter
  virtual int GetNumMissions() const = 0;
  // get particular mission statistics
  virtual IMissionStatistics * GetMission(int nIndex) const = 0;
};

interface ICampaignStatistics : IScenarioStatistics
{
  // get campaign type
  virtual ECampaignType GetType() const = 0;
  // get num chapters, ever started in this campaign
  virtual int GetNumChapters() const = 0;
  // get particular chapter statistics
  virtual IChapterStatistics * GetChapter(int nIndex) const = 0;
};

// ************************************************************************************************************************ //
// **
// ** scenario tracker
// **
// **
// **
// ************************************************************************************************************************ //

interface IScenarioTracker : IRefCount
{
  enum { tidTypeID = MAIN_SCENARIO_TRACKER };

  //
  virtual bool Init(ISingleton *pSingleton) = 0;
  //
  // players
  //
  // add new player with 'nPlayerID'. 
  virtual IPlayerScenarioInfo * AddPlayer(int nPlayerID) = 0;
  // remove player with 'nPlayerID'
  virtual bool RemovePlayer(int nPlayerID) = 0;
  // get player with 'nPlayerID'.
  virtual IPlayerScenarioInfo * GetPlayer(int nPlayerID) const = 0;
  // set user player
  virtual void SetUserPlayer(int nPlayerID) = 0;
  // get user player interface
  virtual IPlayerScenarioInfo * GetUserPlayer() const = 0;
  virtual int GetUserPlayerID() const = 0;
  // iterate through all players
  virtual IPlayerScenarioInfoIterator * CreatePlayerScenarioInfoIterator() const = 0;
  //
  // campaign, chapters, missions
  //
  // start new campaign (for all players)
  virtual void StartCampaign(const std::string &szCampaignName, ECampaignType eType) = 0;
  // start new chapter (and finish previous one)
  virtual bool StartChapter(const std::string &szChapterName) = 0;
  // start new mission
  virtual void StartMission(const std::string &szMissionName) = 0;
  // finish mission
  virtual void FinishMission(EMissionFinishStatus eStatus) = 0;
  // current mission GUID
  virtual const GUID & GetCurrMissionGUID() const = 0;
  // 
  // minimum difficulty tracking
  //
  virtual void UpdateMinimumDifficulty() = 0;
  virtual const std::string & GetMinimumDifficulty() const = 0;
  //
  // campaign random mission templates
  //
  // number of available random mission templates
  virtual int GetNumRandomTemplates() const = 0;
  // get template by number
  virtual const std::string & GetTemplateName(int nIndex) const = 0;
  //

  // removes all random bonuses
  virtual void ClearRandomBonuses(int nDifficulty) = 0;
  // adds a random bonus
  virtual bool AddRandomBonus(int nDifficulty, const std::string &rszRandomBonus) = 0;
  // returns the random bonus, removing it from the list
  virtual std::string GetRandomBonus(int nDifficulty) = 0;

  virtual IScenarioTracker * Duplicate() const = 0;
  virtual int operator&(IDataTree &ss) = 0;
};

#endif // __SCENARIOTRACKER2_H__