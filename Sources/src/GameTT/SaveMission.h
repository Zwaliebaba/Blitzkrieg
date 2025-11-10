#ifndef __SAVE_MISSION_H__
#define __SAVE_MISSION_H__

#pragma once

#include "../Common/InterfaceScreenBase.h"
#include "../Input/InputHelper.h"
#include "iMission.h"

class CInterfaceSaveMission : public CInterfaceScreenBase
{
  OBJECT_NORMAL_METHODS(CInterfaceSaveMission);
  //
  std::vector<std::string> szSaves;
  std::string szProspecitveSave;
  // input
  NInput::CCommandRegistrator savemissionMsgs;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  bool STDCALL StepLocal(bool bAppActive) override;
  void STDCALL DrawAdd() override;
  // disable explicit destruction
  ~CInterfaceSaveMission() override;

  void OnSave();

protected:
  CInterfaceSaveMission() : CInterfaceScreenBase("Current") {}

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
  void STDCALL OnGetFocus(bool bFocus) override;

  void Configure(int nMode);
};

class CICSaveMission : public CInterfaceCommandBase<CInterfaceSaveMission, MISSION_INTERFACE_SAVE_MISSION>
{
  OBJECT_NORMAL_METHODS(CICSaveMission);

  int nType;

  void PreCreate(IMainLoop *pML) override {}

  void PostCreate(IMainLoop *pML, CInterfaceSaveMission *pInterface) override
  {
    pInterface->Configure(nType);
    pML->PushInterface(pInterface);
  }

  //
  CICSaveMission() {}

public:
  void STDCALL Configure(const char *pszConfig) override
  {
    if (!pszConfig) nType = 0;
    else nType = NStr::ToInt(pszConfig);
  }
};

#endif // __SAVE_MISSION_H__