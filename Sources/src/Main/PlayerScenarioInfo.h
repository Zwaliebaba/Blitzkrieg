#ifndef __PLAYERSCENARIOINFO_H__
#define __PLAYERSCENARIOINFO_H__

#pragma once

#include "ScenarioTracker.h"
#include "PlayerSkill.h"
#include "ScenarioStatistics.h"

// ************************************************************************************************************************ //
// **
// **scenario unit info
// **
// **
// **
// ************************************************************************************************************************ //

class CScenarioUnit : public CTRefCount<IScenarioUnit>
{
  OBJECT_SERVICE_METHODS(CScenarioUnit);
  DECLARE_SERIALIZE;
  //
  std::string szRPGStats;// unit's RPG stats
  std::vector<int> values;// unit's values ​​(exp, kills, etc.)
  std::vector<int> currValues;// current mission values
  std::vector<int> valueDiffs;// last mission diffs
  bool bKilled;// unit was killed in mission
  CPtr<IText> pName;// personal localized name
  std::string szNameFileName;// file name with the localized name
  int nScenarioID;// scenario ID
public:
  CScenarioUnit();
  // unit's values
  void SetValue(int nType, int nValue) override;
  void AddValue(int nType, int nValue) override;
  int GetValue(int nType) const override;
  int GetValueDiff(int nType) const override;
  // kill
  void Kill() override { bKilled = true; }
  // personal name
  interface IText * GetName() const override;
  // RPG stats
  void ChangeRPGStats(const std::string &szStatsName) override;
  const std::string & GetRPGStats() const override;
  // scenario ID
  int GetScenarioID() const override { return nScenarioID; }
  //
  // internal management
  //
  bool IsKilled() const { return bKilled; }
  void Reincarnate(bool bLowerLevel);
  void BeginMission();
  void AcceptMission();
  void ClearMission();
  //
  void SetPersonalName(const std::string &szName);
  const std::string &GetPersonalNameFileName() const { return szNameFileName; }
  void SetRPGStats(const std::string &szNewRPGStats) { szRPGStats = szNewRPGStats; }
  void Init(int nID);
  void SetExpToNextLevel();
};

// ************************************************************************************************************************ //
// **
// ** player scenario info
// **
// **
// **
// ************************************************************************************************************************ //

class CPlayerScenarioInfo : public CTRefCount<IPlayerScenarioInfo>
{
  OBJECT_SERVICE_METHODS(CPlayerScenarioInfo);
  DECLARE_SERIALIZE;
  //
  std::wstring wszName;// player name
  CPtr<IText> pNameObject;// player name object
  // side
  std::string szSide;// side (USSR, German, French, Poland, Italy, etc.)
  std::string szGeneralSide;// general side (USSR, German, Allies)
  CPtr<IText> pSideName;// localized side name
  int nDiplomacySide;// diplomacy side [0..2]
  // color on the map
  DWORD dwColor;// color
  // rank & skill
  std::vector<SPlayerSkill> skills;// skills.
  SPlayerRank rank;// current rank
  float fExperience;// players experience
  bool bGainLevel;// is player gained level?
  // units
  using CUnitsList = std::vector<CObj<CScenarioUnit>>;
  CUnitsList units;// all player's units
  std::vector<int> newUnits;// new added units (after mission start)
  // medals
  std::vector<std::string> medalSlots;// slots for medals
  std::vector<int> newMedals;// new came medals
  // upgrades
  std::string szUpgrade;// single upgrade after mission
  std::vector<std::string> depotUpgrades;// depot (endless) upgrades
  std::vector<std::string> depotNewUpgrades;// new depot upgrades
  // statistics
  CObj<CCampaignStatistics> pCampaignStats;// current campaign statistics for this player (main storage for all statistics)
  CPtr<CChapterStatistics> pChapterStats;// current chapter statistics for this player (shortcur from campaign stats)
  CPtr<CMissionStatistics> pMissionStats;// current mission statistics for this player (shortcut from chapter stats)
public:
  CPlayerScenarioInfo();
  void Init();
  // player's name
  void SetName(const std::wstring &wszName) override;
  const std::wstring & GetName() const override;
  IText * GetNameObject() const override;
  //
  // current player side (USSR, German, Poland, French, GB, etc) and general side (USSR, German, Allies)
  //
  // side/general side
  void SetSide(const std::string &szSideName) override;
  const std::string & GetSide() const override;
  const std::string & GetGeneralSide() const override;
  interface IText * GetSideName() const override;
  // diplomacy side [0..2] (0 <=> 1 opponents, 2 - neutral)
  void SetDiplomacySide(int nDiplomacySide) override;
  const int GetDiplomacySide() const override;
  // player color
  void SetColor(DWORD dwColor) override;
  DWORD GetColor() const override;
  //
  // rank & skills
  //
  const struct SPlayerSkill & GetSkill(int nSkill) const override;
  void SetSkill(int nSkill, float fVal) override;
  // player's rank
  const struct SPlayerRank & GetRankInfo() const override;
  void ClearLevelGain() override;
  bool IsGainLevel() const override;
  // set player's experience. 
  bool SetExperience(double fExperience) override;
  //
  // units
  //
  int GetNumUnits() const override;
  IScenarioUnit * GetUnit(int nIndex) const override;
  int GetNumNewUnits() const override;
  IScenarioUnit * GetNewUnit(int nIndex) const override;
  //
  // medals
  //
  // get medal in purticular slot or return 0 if this slot are empty
  const std::string & GetMedalInSlot(int nSlot) const override;
  bool HasMedal(const std::string &szName) const override;
  // new medals
  int GetNumNewMedals() const override;
  const std::string & GetNewMedal(int nIndex) const override;
  //
  // upgrade and depot
  //
  // get available upgrade (can be empty)
  const std::string & GetUpgrade() const override;
  // depot (unlimited) upgrades
  int GetNumDepotUpgrades() const override;
  const std::string & GetDepotUpgrade(int nIndex) const override;
  void OrderDepotUpgrade(int nUpgradeIndex) override;
  int GetNumNewDepotUpgrades() const override;
  const std::string & GetNewDepotUpgrade(int nIndex) const override;
  void ClearNewDepotUpgrade() override;
  //
  // statistics
  //
  // get current campaign stats
  ICampaignStatistics * GetCampaignStats() const override;
  // get current chapter stats
  IChapterStatistics * GetChapterStats() const override;
  // get current mission stats
  IMissionStatistics * GetMissionStats() const override;
  //
  // internal management
  //
  void StartCampaign(CCampaignStatistics *pStats);
  void StartChapter(CChapterStatistics *pStats);
  void StartMission(CMissionStatistics *pStats);
  void FinishMission(EMissionFinishStatus eStatus);
  //
  CScenarioUnit *AddNewSlot(const std::string &szRPGStats);
  void SetUpgrade(const std::string &szUpgradeRPGStats);
  void AddDepotUpgrade(const std::string &szRPGStats);
  void RemoveDepotUpgrade(const std::string &szRPGStats);
  void AddMedal(const std::string &szMedal, int nSlot);
};

#endif // __PLAYERSCENARIOINFO_H__