#ifndef __SAVE_REPLAY_H__
#define __SAVE_REPLAY_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

class CInterfaceSaveReplay : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceSaveReplay);
  // input
  NInput::CCommandRegistrator commandMsgs;
  std::string szSaveReplayFile;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceSaveReplay() override;
  CInterfaceSaveReplay() : CInterfaceInterMission("InterMission") {}
  void CheckEnableOk() const;

  void OnSave();
  void STDCALL OnGetFocus(bool bFocus) override;

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
};

class CICSaveReplay : public CInterfaceCommandBase<CInterfaceSaveReplay, MISSION_INTERFACE_SAVE_REPLAY>
{
  OBJECT_NORMAL_METHODS(CICSaveReplay);

  void PreCreate(IMainLoop *pML) override {}
  void PostCreate(IMainLoop *pML, CInterfaceSaveReplay *pISR) override { pML->PushInterface(pISR); }
  //
  CICSaveReplay() {}
};

#endif		// __SAVE_REPLAY_H__