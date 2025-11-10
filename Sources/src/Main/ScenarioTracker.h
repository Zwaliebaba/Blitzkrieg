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
  virtual bool STDCALL IsHelpCalled(int nInterfaceTypeID, int nHelpNumber) const = 0;
  virtual void STDCALL HelpCalled(int nInterfaceTypeID, int nHelpNumber) = 0;
  //
  // cutscenes availability
  //
  virtual void STDCALL AddCutScene(const std::string &szCutSceneName) = 0;
  virtual int STDCALL GetNumCutScenes() const = 0;
  virtual const std::string & STDCALL GetCutScene(int nIndex) const = 0;
  //
  // templates usage statistics
  //
  virtual void STDCALL AddUsedTemplate(const std::string &rszTemplate, int nTemplateWeight, const std::string &rszGraph, int nGraphWeight, int nAngle, int nAngleWeight) = 0;
  virtual int STDCALL GetUsedTemplates(const std::string &rszTemplate) = 0;
  virtual int STDCALL GetUsedTemplateGraphs(const std::string &rszTemplate, const std::string &rszGraph) = 0;
  // 0...3
  virtual int STDCALL GetUsedAngles(int nAngle) = 0;
  //
  // users relations for chat
  //
  virtual void STDCALL SetChatRelation(const wchar_t *pwszNick, enum EPlayerRelation eRelation) = 0;
  virtual const enum EPlayerRelation STDCALL GetChatRelation(const wchar_t *pwszNick) = 0;
  //
  // current MOD
  // gets and returns dirname (subdirectory in Mods directory)
  // returns with '\\' at the end.
  //
  virtual void STDCALL SetMOD(const std::string &szMOD) = 0;
  virtual const std::string & STDCALL GetMOD() const = 0;
  //
  // loads counters, based on GUID for each mission
  //
  virtual void STDCALL RegisterLoad(const GUID &guid) = 0;
  virtual int STDCALL GetLoadCounter(const GUID &guid) const = 0;

  //
  // user values ​​(stored in profile)
  //
  virtual void STDCALL AddVar(const char *pszValueName, int nValue) = 0;
  virtual int STDCALL GetVar(const char *pszValueName, int nDefValue) const = 0;
  virtual void STDCALL RemoveVar(const char *pszValueName) = 0;

  //
  // serialization & repair
  //
  virtual bool STDCALL IsChanged() const = 0;
  virtual void STDCALL SerializeConfig(IDataTree *pSS) = 0;
  virtual void STDCALL Repair(IDataTree *pSS, bool bToDefault) = 0;
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
  virtual void STDCALL SetValue(int nType, int nValue) = 0;
  virtual void STDCALL AddValue(int nType, int nValue) = 0;
  virtual int STDCALL GetValue(int nType) const = 0;
  virtual int STDCALL GetValueDiff(int nType) const = 0;
  // kill
  virtual void STDCALL Kill() = 0;
  // personal name
  virtual interface IText * STDCALL GetName() const = 0;
  // RPG stats
  virtual void STDCALL ChangeRPGStats(const std::string &szStatsName) = 0;
  virtual const std::string & STDCALL GetRPGStats() const = 0;
  // scenario ID
  virtual int STDCALL GetScenarioID() const = 0;
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
  virtual void STDCALL SetName(const std::wstring &wszName) = 0;
  virtual const std::wstring & STDCALL GetName() const = 0;
  virtual IText * STDCALL GetNameObject() const = 0;
  //
  // current player side (USSR, German, Poland, French, GB, etc) and general side (USSR, German, Allies)
  //
  // side/general side
  virtual void STDCALL SetSide(const std::string &szSideName) = 0;
  virtual const std::string & STDCALL GetSide() const = 0;
  virtual const std::string & STDCALL GetGeneralSide() const = 0;
  virtual interface IText * STDCALL GetSideName() const = 0;
  // diplomacy side [0..2] (0 <=> 1 opponents, 2 - neutral)
  virtual void STDCALL SetDiplomacySide(int nDiplomacySide) = 0;
  virtual const int STDCALL GetDiplomacySide() const = 0;
  // player color
  virtual void STDCALL SetColor(DWORD dwColor) = 0;
  virtual DWORD STDCALL GetColor() const = 0;
  //
  // rank & skills
  //
  virtual const struct SPlayerSkill & STDCALL GetSkill(int nSkill) const = 0;
  virtual void STDCALL SetSkill(int nSkill, float fVal) = 0;
  // player's rank
  virtual const struct SPlayerRank & STDCALL GetRankInfo() const = 0;
  virtual void STDCALL ClearLevelGain() = 0;
  virtual bool STDCALL IsGainLevel() const = 0;
  // set player's experience. 
  virtual bool STDCALL SetExperience(double fExperience) = 0;
  //
  // units
  //
  virtual int STDCALL GetNumUnits() const = 0;
  virtual IScenarioUnit * STDCALL GetUnit(int nIndex) const = 0;
  virtual int STDCALL GetNumNewUnits() const = 0;
  virtual IScenarioUnit * STDCALL GetNewUnit(int nIndex) const = 0;
  //
  // medals
  //
  // get medal in purticular slot or return 0 if this slot are empty
  virtual const std::string & STDCALL GetMedalInSlot(int nSlot) const = 0;
  virtual bool STDCALL HasMedal(const std::string &szName) const = 0;
  // new medals
  virtual int STDCALL GetNumNewMedals() const = 0;
  virtual const std::string & STDCALL GetNewMedal(int nIndex) const = 0;
  //
  // upgrade and depot
  //
  // get available upgrade (can be empty)
  virtual const std::string & STDCALL GetUpgrade() const = 0;
  // depot (unlimited) upgrades
  virtual int STDCALL GetNumDepotUpgrades() const = 0;
  virtual const std::string & STDCALL GetDepotUpgrade(int nIndex) const = 0;
  virtual void STDCALL OrderDepotUpgrade(int nUpgradeIndex) = 0;
  virtual int STDCALL GetNumNewDepotUpgrades() const = 0;
  virtual const std::string & STDCALL GetNewDepotUpgrade(int nIndex) const = 0;
  virtual void STDCALL ClearNewDepotUpgrade() = 0;
  //
  //
  //
  // get current campaign stats
  virtual interface ICampaignStatistics * STDCALL GetCampaignStats() const = 0;
  // get current chapter stats
  virtual interface IChapterStatistics * STDCALL GetChapterStats() const = 0;
  // get current mission stats
  virtual interface IMissionStatistics * STDCALL GetMissionStats() const = 0;
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
  virtual void STDCALL Next() = 0;
  // check, if we've reached end?
  virtual bool STDCALL IsEnd() const = 0;
  // get current iterator's player scenario info
  virtual IPlayerScenarioInfo * STDCALL Get() const = 0;
  // get current iterator's player ID
  virtual int STDCALL GetID() const = 0;
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
  virtual const std::string & STDCALL GetName() const = 0;
  // retrieve statistics value by type
  virtual int STDCALL GetValue(int nType) const = 0;
  // number of killed in action (KIA)
  virtual int STDCALL GetNumKIA() const = 0;
  // KIA name (file name with localized name)
  virtual const std::string & STDCALL GetKIAName(int nIndex) const = 0;
  // KIA new name (file name with localized name)
  virtual const std::string & STDCALL GetKIANewName(int nIndex) const = 0;
  // KIA RPG stats
  virtual const std::string & STDCALL GetKIAStats(int nIndex) const = 0;
};

interface IMissionStatistics : IScenarioStatistics
{
  // add (increment) value
  virtual void STDCALL AddValue(int nType, int nValue) = 0;
  // set value directly (override)
  virtual void STDCALL SetValue(int nType, int nValue) = 0;
  // get mission finish status
  virtual EMissionFinishStatus STDCALL GetFinishStatus() const = 0;
};

interface IChapterStatistics : IScenarioStatistics
{
  // get num missions, ever started in this chapter
  virtual int STDCALL GetNumMissions() const = 0;
  // get particular mission statistics
  virtual IMissionStatistics * STDCALL GetMission(int nIndex) const = 0;
};

interface ICampaignStatistics : IScenarioStatistics
{
  // get campaign type
  virtual ECampaignType STDCALL GetType() const = 0;
  // get num chapters, ever started in this campaign
  virtual int STDCALL GetNumChapters() const = 0;
  // get particular chapter statistics
  virtual IChapterStatistics * STDCALL GetChapter(int nIndex) const = 0;
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
  virtual bool STDCALL Init(ISingleton *pSingleton) = 0;
  //
  // players
  //
  // add new player with 'nPlayerID'. 
  virtual IPlayerScenarioInfo * STDCALL AddPlayer(int nPlayerID) = 0;
  // remove player with 'nPlayerID'
  virtual bool STDCALL RemovePlayer(int nPlayerID) = 0;
  // get player with 'nPlayerID'.
  virtual IPlayerScenarioInfo * STDCALL GetPlayer(int nPlayerID) const = 0;
  // set user player
  virtual void STDCALL SetUserPlayer(int nPlayerID) = 0;
  // get user player interface
  virtual IPlayerScenarioInfo * STDCALL GetUserPlayer() const = 0;
  virtual int STDCALL GetUserPlayerID() const = 0;
  // iterate through all players
  virtual IPlayerScenarioInfoIterator * STDCALL CreatePlayerScenarioInfoIterator() const = 0;
  //
  // campaign, chapters, missions
  //
  // start new campaign (for all players)
  virtual void STDCALL StartCampaign(const std::string &szCampaignName, ECampaignType eType) = 0;
  // start new chapter (and finish previous one)
  virtual bool STDCALL StartChapter(const std::string &szChapterName) = 0;
  // start new mission
  virtual void STDCALL StartMission(const std::string &szMissionName) = 0;
  // finish mission
  virtual void STDCALL FinishMission(EMissionFinishStatus eStatus) = 0;
  // current mission GUID
  virtual const GUID & STDCALL GetCurrMissionGUID() const = 0;
  // 
  // minimum difficulty tracking
  //
  virtual void STDCALL UpdateMinimumDifficulty() = 0;
  virtual const std::string & STDCALL GetMinimumDifficulty() const = 0;
  //
  // campaign random mission templates
  //
  // number of available random mission templates
  virtual int STDCALL GetNumRandomTemplates() const = 0;
  // get template by number
  virtual const std::string & STDCALL GetTemplateName(int nIndex) const = 0;
  //

  // removes all random bonuses
  virtual void STDCALL ClearRandomBonuses(int nDifficulty) = 0;
  // adds a random bonus
  virtual bool STDCALL AddRandomBonus(int nDifficulty, const std::string &rszRandomBonus) = 0;
  // returns the random bonus, removing it from the list
  virtual std::string STDCALL GetRandomBonus(int nDifficulty) = 0;

  virtual IScenarioTracker * STDCALL Duplicate() const = 0;
  virtual int STDCALL operator&(IDataTree &ss) = 0;
};

#endif // __SCENARIOTRACKER2_H__