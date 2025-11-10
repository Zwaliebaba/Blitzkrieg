#ifndef __INTERFACEOPTIONSSETTINGS_H__
#define __INTERFACEOPTIONSSETTINGS_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

class COptionsListWrapper;

class CInterfaceOptionsSettings : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceOptionsSettings);

  std::vector<CPtr<COptionsListWrapper>> optionsLists;

  int nActive;// current active division
  int nMaxDivision;// total number of divisions.
  int nMinDifficulty;

  // input
  NInput::CCommandRegistrator commandMsgs;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;

  ~CInterfaceOptionsSettings() override {}
  CInterfaceOptionsSettings() : CInterfaceInterMission(/* "InterMission" */"Current"), nActive(-1), nMaxDivision(0) {}

  void SuspendAILogic(bool bSuspend) override;
  void OnChangeDivision(int nDivision);
  void Close();
  bool OpenCurtains() override;

public:
  void STDCALL Done() override;
  bool STDCALL Init() override;
  void Create();

};

class CICOptionsSettings : public CInterfaceCommandBase<CInterfaceOptionsSettings, MISSION_INTERFACE_OPTIONSSETTINGS>
{
  OBJECT_NORMAL_METHODS(CICOptionsSettings);

  void PreCreate(IMainLoop *pML) override {}

  void PostCreate(IMainLoop *pML, CInterfaceOptionsSettings *pEI) override
  {
    pEI->Create();
    pML->PushInterface(pEI);
  }

  //
  CICOptionsSettings() {}

public:
  void STDCALL Configure(const char *pszConfig) override {}
};

#endif // __INTERFACEOPTIONSSETTINGS_H__