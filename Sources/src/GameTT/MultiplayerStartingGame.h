#pragma once

#include "InterMission.h"
#include "iMission.h"
#include "MultiplayerCommandManager.h"
#include "ListControlWrapper.h"
#include "ChatWrapper.h"

class CMapSettingsWrapper;

class CInterfaceMPStartingGame : public CInterfaceMultiplayerScreen, public IWhisper
{
  OBJECT_NORMAL_METHODS(CInterfaceMPStartingGame);

  CPtr<CMapSettingsWrapper> pMapSettingsWrapper;
  CListControlWrapper<SUIPlayerInfo, int> playerList;
  CChatWrapper chat;
  bool bFirstConfiguration;// first receive of configuration parameters.

  CPtr<SUIStagingRoomConfigure> pConfiguration;// configuration of this room
  NInput::CCommandRegistrator commandMsgs;
  std::string szMapName;
  bool bStarted;// game launched

  CInterfaceMPStartingGame() : CInterfaceMultiplayerScreen("InterMission") {}
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;

  // return true if need process commands further
  bool ProcessMPCommand(const SToUICommand &cmd);
  void UpdateButtons();
  void AddOrUpdatePlayer(SUIPlayerInfo *pPlayerInfo);
  void ConfigureStagingRoom(SUIStagingRoomConfigure *pInfo);
  void OnNewServerSettings(const SMultiplayerGameSettings &serverSettings, bool bVisialNotify);

  void PlayerLeft(const SUIPlayerInfo *pInfo);
  void PlayerKicked(const SUIPlayerInfo *pInfo);
  void DeletePlayer(const SUIPlayerInfo *pPlayerInfo);

  void AddMessageToChat(const SChatMessage *pChatMessage);

  void STDCALL OnGetFocus(bool bFocus) override;
  void OnStart(bool bForced);
  void NotifyOptionsChanged();

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
  virtual void STDCALL SetParams(const char *pszParams);
  void STDCALL Done() override;
  // IWhisper
  const wchar_t *GetDestinationName() override;
};

class CICMultyplayerStartingGame : public CInterfaceCommandBase<CInterfaceMPStartingGame, MISSION_INTERFACE_MULTIPLAYER_STARTINGGAME>
{
  DECLARE_SERIALIZE;
  OBJECT_NORMAL_METHODS(CICMultyplayerStartingGame);

  std::string szParams;

  void PreCreate(IMainLoop *pML) override { pML->ResetStack(); }

  void PostCreate(IMainLoop *pML, CInterfaceMPStartingGame *pIMM) override
  {
    pIMM->SetParams(szParams.c_str());
    pML->PushInterface(pIMM);
  }

public:
  void STDCALL Configure(const char *pszConfig) override { if (pszConfig) szParams = pszConfig; }
};
