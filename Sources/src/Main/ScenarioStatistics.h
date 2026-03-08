#ifndef __SCENARIOSTATISTICS_H__
#define __SCENARIOSTATISTICS_H__

#pragma once

#include "ScenarioTracker.h"

// ************************************************************************************************************************ //
// **
// ** mission statistics
// **
// **
// **
// ************************************************************************************************************************ //

class CMissionStatistics : public CTRefCount<IMissionStatistics>
{
  OBJECT_SERVICE_METHODS(CMissionStatistics);
  DECLARE_SERIALIZE;

  //
  struct SKIAUnit
  {
    std::string szOldName;
    std::string szNewName;
    std::string szRPGStats;
    //
    SKIAUnit() {}

    SKIAUnit(const std::string &_szOldName, const std::string &_szNewName, const std::string &_szRPGStats)
      : szOldName(_szOldName), szNewName(_szNewName), szRPGStats(_szRPGStats) {}

    //
    int operator&(IStructureSaver &ss)
    {
      CSaverAccessor saver = &ss;
      saver.Add(1, &szOldName);
      saver.Add(2, &szNewName);
      saver.Add(3, &szRPGStats);
      return 0;
    }
  };

  //
  std::string szName;// game stats name
  std::vector<int> values;// mission values ​​statistics
  EMissionFinishStatus eStatus;// finish status
  std::vector<SKIAUnit> kiaUnits;// killed in this mission
public:
  CMissionStatistics();
  // get mission/chapter/campaign name
  const std::string & STDCALL GetName() const override;
  // retrieve statistics value by type
  int STDCALL GetValue(int nType) const override;
  // add (increment) value
  void STDCALL AddValue(int nType, int nValue) override;
  // set value directly (override)
  void STDCALL SetValue(int nType, int nValue) override;
  // get mission finish status
  EMissionFinishStatus STDCALL GetFinishStatus() const override;
  // number of killed in action (KIA)
  int STDCALL GetNumKIA() const override { return kiaUnits.size(); }
  // KIA name (file name with localized name)
  const std::string & STDCALL GetKIAName(int nIndex) const override;
  // KIA new name (file name with localized name)
  const std::string & STDCALL GetKIANewName(int nIndex) const override;
  // KIA RPG stats
  const std::string & STDCALL GetKIAStats(int nIndex) const override;
  //
  // internal management
  //
  void SetName(const std::string &_szName) { szName = _szName; }
  void SetFinishStatus(const EMissionFinishStatus _eStatus) { eStatus = _eStatus; }
  void AddKIAUnit(const std::string &szOldName, const std::string &szNewName, const std::string &szRPGStats) { kiaUnits.push_back(SKIAUnit(szOldName, szNewName, szRPGStats)); }
};

// ************************************************************************************************************************ //
// **
// ** chapter statistics
// **
// **
// **
// ************************************************************************************************************************ //

class CChapterStatistics : public CTRefCount<IChapterStatistics>
{
  OBJECT_SERVICE_METHODS(CChapterStatistics);
  DECLARE_SERIALIZE;
  //
  std::string szName;// game stats name
  using CMissionStatisticsList = std::vector<CObj<CMissionStatistics>>;
  CMissionStatisticsList missions;// all missions in this chapter
public:
  // get mission/chapter/campaign name
  const std::string & STDCALL GetName() const override;
  // retrieve statistics value by type
  int STDCALL GetValue(int nType) const override;
  // get num missions, ever started in this chapter
  int STDCALL GetNumMissions() const override;
  // get particular mission statistics
  IMissionStatistics * STDCALL GetMission(int nIndex) const override;
  // number of killed in action (KIA)
  int STDCALL GetNumKIA() const override;
  // KIA name (file name with localized name)
  const std::string & STDCALL GetKIAName(int nIndex) const override;
  // KIA new name (file name with localized name)
  const std::string & STDCALL GetKIANewName(int nIndex) const override;
  // KIA RPG stats
  const std::string & STDCALL GetKIAStats(int nIndex) const override;
  //
  // internal management
  //
  void SetName(const std::string &_szName) { szName = _szName; }
  void AddMission(CMissionStatistics *pMission) { missions.push_back(pMission); }
};

// ************************************************************************************************************************ //
// **
// ** campaign statistics
// **
// **
// **
// ************************************************************************************************************************ //

class CCampaignStatistics : public CTRefCount<ICampaignStatistics>
{
  OBJECT_SERVICE_METHODS(CCampaignStatistics);
  DECLARE_SERIALIZE;
  //
  std::string szName;// game stats name
  using CChapterStatisticsList = std::vector<CObj<CChapterStatistics>>;
  CChapterStatisticsList chapters;// all chapters in this campaign
  ECampaignType eType;// campaign type
public:
  // get mission/chapter/campaign name
  const std::string & STDCALL GetName() const override;
  // get campaign type
  ECampaignType STDCALL GetType() const override;
  // retrieve statistics value by type
  int STDCALL GetValue(int nType) const override;
  // get num chapters, ever started in this campaign
  int STDCALL GetNumChapters() const override;
  // get particular chapter statistics
  IChapterStatistics * STDCALL GetChapter(int nIndex) const override;
  // number of killed in action (KIA)
  int STDCALL GetNumKIA() const override;
  // KIA name (file name with localized name)
  const std::string & STDCALL GetKIAName(int nIndex) const override;
  // KIA new name (file name with localized name)
  const std::string & STDCALL GetKIANewName(int nIndex) const override;
  // KIA RPG stats
  const std::string & STDCALL GetKIAStats(int nIndex) const override;
  //
  // internal management
  //
  void SetName(const std::string &_szName, ECampaignType _eType);
  void AddChapter(CChapterStatistics *pChapter) { chapters.push_back(pChapter); }
};

#endif // __SCENARIOSTATISTICS_H__