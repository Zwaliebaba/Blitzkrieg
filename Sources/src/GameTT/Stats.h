#ifndef __IM_STATS_H__
#define __IM_STATS_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"
#include "../Main/ScenarioTrackerTypes.h"
#include "../Main/ScenarioTracker.h"

class CInterfaceStats : public CInterfaceInterMission
{
  DECLARE_SERIALIZE;
  OBJECT_NORMAL_METHODS(CInterfaceStats);

public:
  enum EPartyInfoType
  {
    E_NONE,
    E_SUMMARY,
    E_BEST_NO_PLAYER_DISPLAY,
    E_SUMMARY_DEVIDED_BY_1000,
  };

  enum EStatisticsGameType
  {
    EST_BOTH,
    EST_MULTIPLAYER,
    EST_SINGLEPLAYER,
  };

  // for custom sorter
  class CSorter : public IUIListSorter
  {
    OBJECT_NORMAL_METHODS(CSorter);

  public:
    bool STDCALL operator()(int nSortColumn, const IUIListRow *pRow1, const IUIListRow *pRow2, bool bForward) const override;
  };

  using CPlayerFullInfo = std::pair<CPtr<IPlayerScenarioInfo>, CPtr<IScenarioStatistics>>;
  using CPlayersFullInformation = std::vector<CPlayerFullInfo>;

  // configuration for player's stats
  struct SStatConfugure
  {
    /* EScenarioTrackerMissionTypes */
    int eType[2];
    /* EStatisticsGameType */
    int eStatType;
    /* EPartyInfoType */
    int eAccumulateType;// type of accumulate party stats
    int nIndexToCountBest;// best will be counted by this value
    bool bLeaderIsGreatest;// leader is count by greatest value if true (otherwise by smallest)

    SStatConfugure() {}
    SStatConfugure(int _eStatType,
                   /* EPartyInfoType */ int _eAccumulateType,
                   int _eType1, int _eType2 = -1,
                   int _nIndexToCountBest = 0,
                   bool bGreatest = true);

    bool IsToDisplay(bool bMultiplayerGame) const;
  };

  // info for player statistics
  struct SPlayerStatInfo
  {
    double fVal[2];

    void Init(const CPlayerFullInfo &playerInfo, const SStatConfugure &config);
    std::wstring GetValue(const SStatConfugure &config) const;
    std::wstring GetValueToSort(const SStatConfugure &config) const;
  };

  // party info (gathered through all players)
  class SPartyInfo
  {
    double fVal[2];// summ val for party
    double fBestVal[2];// best is by the first val.
    int nBestIndex;// index of best player
  public:
    SPartyInfo() : nBestIndex(-1) {}

    void Init(const CPlayersFullInformation &info, const SStatConfugure &config);
    // double GetPartyVal() const { return fVal; 
    std::wstring GetPartyVal(const SStatConfugure &config) const;
    std::wstring GetValForSort(const SStatConfugure &config) const;
    int GetLeader() const { return nBestIndex; }// leader of that statistics
    double GetLeaderValue(const SStatConfugure &config) const { return fBestVal[config.nIndexToCountBest]; }
  };

  // common stats
  class CCommonStats
  {
    int nTime;
    int nObjectivesCompleted;
    int nDifficulty;
    // int nObjectivesFailed;
    int nUpgrades;
    int nSaves;

  public:
    void Init(const CPlayerFullInfo &info);
    int GetNStats(bool bMultiplayer) const;
    std::string GetStatTitleKey(int nIndex) const;
    std::wstring GetStatValue(int nIndex) const;
  };

private:
  CPtr<CSorter> pSorter;
  // for player's stats configuration
  std::vector<SStatConfugure> playerStatsConfigure;

  // the collected players
  std::vector<CPlayersFullInformation> playerInfos;

  // input
  NInput::CCommandRegistrator commandMsgs;

  std::vector<CPtr<IUIListControl>> pPartyList;

  bool bPopupsShowed;


  bool bTutorialWindow;
  bool bCheckReplay;
  int m_nStatsType;
  int nMedalIterator;// current medal index
  bool bStatsShown;
  bool bUpgradesShown;
  bool bNewUnitsShown;
  bool bNextChapterShown;
  bool bPlayerRankShown;
  bool bLastFullScreen;// variable for the operation of curtains, if true then the last time the full screen interface was displayed

  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceStats() override;

  CInterfaceStats() : CInterfaceInterMission("InterMission"), playerInfos(2), pPartyList(2),
                      bCheckReplay(false), m_nStatsType(0), nMedalIterator(0), bStatsShown(false), bUpgradesShown(false),
                      bNewUnitsShown(false), bNextChapterShown(false), bPlayerRankShown(false), bLastFullScreen(false) {}

  void InitSorter();
  void AquireLists();
  // stores statistics
  void CollectPlayerStats(interface IPlayerScenarioInfo *pPlayer, interface IScenarioStatistics *pStats);
  // fills list with values
  void RepositionList();
  void FillCommonStatsList(bool bMultiplayer, const CCommonStats &commonStats);

  void OnDemoversionExit();

public:
  bool STDCALL Init() override;
  void STDCALL Done() override;
  void STDCALL OnGetFocus(bool bFocus) override;
  void Create(int nStatsType);
};

class CICStats : public CInterfaceCommandBase<CInterfaceStats, MISSION_INTERFACE_STATS>
{
  OBJECT_NORMAL_METHODS(CICStats);

  int /* EStatsComplexity */ nCurrentMissionStats;

  void PreCreate(IMainLoop *pML) override;
  void PostCreate(IMainLoop *pML, CInterfaceStats *pIS) override;
  //
  CICStats() {}

public:
  void STDCALL Configure(const char *pszConfig) override;
};

#endif		// __IM_STATS_H__