#ifndef __PLAYER_GAIN_LEVEL_H__
#define __PLAYER_GAIN_LEVEL_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

class CInterfacePlayerGainLevel : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfacePlayerGainLevel);
  // input
  NInput::CCommandRegistrator commandMsgs;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfacePlayerGainLevel() override;
  CInterfacePlayerGainLevel() : CInterfaceInterMission("Current") {}

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
};

class CICPlayerGainLevel : public CInterfaceCommandBase<CInterfacePlayerGainLevel, MISSION_INTERFACE_PLAYER_GAIN_LEVEL>
{
  OBJECT_NORMAL_METHODS(CICPlayerGainLevel);

  void PreCreate(IMainLoop *pML) override {}
  void PostCreate(IMainLoop *pML, CInterfacePlayerGainLevel *pISM) override;
  //
  CICPlayerGainLevel() {}
};

#endif		// __PLAYER_GAIN_LEVEL_H__