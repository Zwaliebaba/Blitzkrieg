#ifndef __INTERFACEMPCHAT_H__
#define __INTERFACEMPCHAT_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"
#include "MultiplayerCommandManager.h"
#include "ListControlWrapper.h"
#include "ChatWrapper.h"

class CInterfaceMPChat : public CInterfaceMultiplayerScreen, public IWhisper
{
  OBJECT_NORMAL_METHODS(CInterfaceMPChat);

  NInput::CCommandRegistrator commandMsgs;
  // player list management
  CListControlWrapper<SUIChatPlayerInfo, std::wstring> playerList;

  CChatWrapper chat;

  CPtr<IUIDialog> pDialog;// for editing friend info
  CPtr<SUIChatPlayerInfo> pCurEdittedInfo;// current friend
  NTimer::STime timeLastAwayPressed;
  bool bAwayPressed;
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  CInterfaceMPChat() : CInterfaceMultiplayerScreen("InterMission"), timeLastAwayPressed(0), bAwayPressed(false) {}

  void SendTextFromEditBox(bool bWhisper = false);
  bool ProcessMPCommand(SToUICommand &cmd);
  void UpdateButtons();

  void ShowPlayerInfo(SUIChatPlayerInfo *pInfo);
  void OnPlayerInfoOk();
  void AddPlayer(SUIChatPlayerInfo *pInfo);
  bool STDCALL StepLocal(bool bAppActive) override;

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;

  // IWhisper
  const WORD *GetDestinationName() override;
};

class CICMultyplayerChat : public CInterfaceCommandBase<CInterfaceMPChat, MISSION_INTERFACE_MULTYPLAYER_CHAT>
{
  OBJECT_NORMAL_METHODS(CICMultyplayerChat);

  void PreCreate(IMainLoop *pML) override { pML->ResetStack(); }

  void PostCreate(IMainLoop *pML, CInterfaceMPChat *pIMM) override { pML->PushInterface(pIMM); }
};


#endif // __INTERFACEMPCHAT_H__