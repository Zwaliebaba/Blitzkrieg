#ifndef __IM_ENCYCLOPEDIA_H__
#define __IM_ENCYCLOPEDIA_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

struct SMedalStats;
struct SUnitBaseRPGStats;

class CInterfaceEncyclopedia : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceEncyclopedia);
  // input
  NInput::CCommandRegistrator commandMsgs;
  bool bFinished;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceEncyclopedia() override;
  CInterfaceEncyclopedia() : CInterfaceInterMission("InterMission"), bFinished(false) {}

  void LoadMedalInfo(const SMedalStats *pMedalStats, std::string *pszTextureFileName, std::wstring *pszTitle, std::wstring *pDesc);
  void LoadUnitInfo(const SUnitBaseRPGStats *pUnitStats, std::string *pszTextureFileName, std::wstring *pszTitle, std::wstring *pDesc, std::wstring *pStatistics);

public:
  bool STDCALL Init() override;
  void STDCALL Done() override;
  void Create(int nType, const char *pszName);
};

class CICEncyclopedia : public CInterfaceCommandBase<CInterfaceEncyclopedia, MISSION_INTERFACE_ENCYCLOPEDIA>
{
  OBJECT_NORMAL_METHODS(CICEncyclopedia);

  int nType;
  std::string szName;

  void PreCreate(IMainLoop *pML) override {}
  void PostCreate(IMainLoop *pML, CInterfaceEncyclopedia *pEI) override;
  //
  CICEncyclopedia() {}

public:
  void STDCALL Configure(const char *pszConfig) override;
};

#endif		// __IM_ENCYCLOPEDIA_H__