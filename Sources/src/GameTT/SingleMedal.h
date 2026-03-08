#ifndef __IM_SINGLE_MEDAL_H__
#define __IM_SINGLE_MEDAL_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

class CInterfaceSingleMedal : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceSingleMedal);
  std::string szMedalName;
  // input
  NInput::CCommandRegistrator commandMsgs;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceSingleMedal() override;
  CInterfaceSingleMedal() : CInterfaceInterMission("Current") {}

public:
  bool STDCALL Init() override;
  void Create(const char *pszName);
};

class CICSingleMedal : public CInterfaceCommandBase<CInterfaceSingleMedal, MISSION_INTERFACE_SINGLE_MEDAL>
{
  OBJECT_NORMAL_METHODS(CICSingleMedal);
  std::string szName;

  void PreCreate(IMainLoop *pML) override {}
  void PostCreate(IMainLoop *pML, CInterfaceSingleMedal *pISM) override;
  //
  CICSingleMedal() {}

public:
  void STDCALL Configure(const char *pszConfig) override;
};

#endif		// __IM_SINGLE_MEDAL_H__