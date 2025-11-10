#ifndef __USERPROFILE_H__
#define __USERPROFILE_H__

#pragma once

#include "ScenarioTracker.h"

struct STemplateUsage
{
  int nCount;
  std::unordered_map<std::string, int> graphs;
  //
  int operator&(IDataTree &ss)
  {
    CTreeAccessor saver = &ss;
    saver.Add("Count", &nCount);
    saver.Add("Graphs", &graphs);
    return 0;
  }
};


struct SGUIDHashFunc
{
  unsigned long operator()(const GUID &guid) const
  {
    const unsigned long uCode = guid.Data1 + guid.Data2 + guid.Data3 +
                                guid.Data4[0] + guid.Data4[1] + guid.Data4[2] + guid.Data4[3] +
                                guid.Data4[4] + guid.Data4[5] + guid.Data4[6] + guid.Data4[7];
    return uCode;
  }
};

struct SGUIDEqual
{
  bool operator()(const GUID &g1, const GUID &g2) const { return memcmp(&g1, &g2, sizeof(g1)) == 0; }
};

struct SLoadCounter
{
  GUID guid;// mission GUID
  int nCounter;// loads counter for this mission
  //
  SLoadCounter()
    : nCounter(0) {}

  SLoadCounter(const GUID &_guid, const int _nCounter)
    : guid(_guid), nCounter(_nCounter) {}

  //
  int operator&(IDataTree &ss)
  {
    CTreeAccessor saver = &ss;
    saver.AddRawData("GUID", &guid, sizeof(guid));
    saver.Add("Counter", &nCounter);
    return 0;
  }
};

class CUserProfile : public CTRefCount<IUserProfile>
{
  OBJECT_SERVICE_METHODS(CUserProfile);

  // user variables
  using CVariables = std::unordered_map<std::string, int>;
  CVariables variables;
  // help screens tracking
  // std::list<int> helpscreens;					
  using CHelpscenes = std::unordered_map<int/* interface ID */, WORD/* help mask */>;
  CHelpscenes helpscreens;
  // cutscenes
  std::vector<std::string> cutscenes;// available cutscenes
  // templates usage statistics
  using CTemplateUsageMap = std::unordered_map<std::string, STemplateUsage>;
  CTemplateUsageMap templates;// templates usage statistics
  std::vector<int> templateAngles;// angles usage statistics
  // chat relations
  using CChatRelations = std::unordered_map<std::wstring, enum EPlayerRelation>;
  CChatRelations chatRelations;
  // MOD support
  std::string szMOD;// MOD name
  // load count
  using CLoadsMap = std::unordered_map<GUID, int, SGUIDHashFunc, SGUIDEqual>;
  CLoadsMap loadCounters;
  //
  bool bChanged;// was user profile changed?
  //
  void SetChanged() { bChanged = true; }

public:
  CUserProfile() : bChanged(false) { templateAngles.resize(4, 0); }
  // 
  // help screens tracker part
  //
  bool STDCALL IsHelpCalled(int nInterfaceTypeID, int nHelpNumber) const override;
  void STDCALL HelpCalled(int nInterfaceTypeID, int nHelpNumber) override;
  //
  // cutscenes availability
  //
  void STDCALL AddCutScene(const std::string &szCutSceneName) override;
  int STDCALL GetNumCutScenes() const override;
  const std::string & STDCALL GetCutScene(int nIndex) const override;
  //
  // templates usage statistics
  //
  void STDCALL AddUsedTemplate(const std::string &rszTemplate, int nTemplateWeight, const std::string &rszGraph, int nGraphWeight, int nAngle, int nAngleWeight) override;
  int STDCALL GetUsedTemplates(const std::string &rszTemplate) override;
  int STDCALL GetUsedTemplateGraphs(const std::string &rszTemplate, const std::string &rszGraph) override;
  // 0...3
  int STDCALL GetUsedAngles(int nAngle) override;
  //
  // user relationships - for chat
  //
  void STDCALL SetChatRelation(const wchar_t *pwszNick, enum EPlayerRelation nRelation) override;
  const enum EPlayerRelation STDCALL GetChatRelation(const wchar_t *pwszNick) override;
  //
  // current MOD
  //
  void STDCALL SetMOD(const std::string &_szMOD) override;
  const std::string & STDCALL GetMOD() const override;
  //
  // loads counters, based on GUID for each mission
  //
  void STDCALL RegisterLoad(const GUID &guid) override;
  int STDCALL GetLoadCounter(const GUID &guid) const override;

  //
  // user values ​​(stored in profile)
  //
  void STDCALL AddVar(const char *pszValueName, int nValue) override;
  int STDCALL GetVar(const char *pszValueName, int nDefValue) const override;
  void STDCALL RemoveVar(const char *pszValueName) override;
  //
  // serialization & repair
  //
  bool STDCALL IsChanged() const override;
  void STDCALL SerializeConfig(IDataTree *pSS) override;
  void STDCALL Repair(IDataTree *pSS, bool bToDefault) override;
};

#endif // __USERPROFILE_H__