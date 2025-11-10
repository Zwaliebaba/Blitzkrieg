#ifndef __INTERFACEMPCREATEGAME_H__
#define __INTERFACEMPCREATEGAME_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"
#include "MultiplayerCommandManager.h"
#include "ListControlWrapper.h"
#include "MapSettingsWrapper.h"
#include "UIMapINfo.h"

#include "../Main/Transceiver.h"

class CInterfaceMPCreateGame : public CInterfaceMultiplayerScreen
{
  OBJECT_NORMAL_METHODS(CInterfaceMPCreateGame);

public:
  struct SComparePredicate
  {
    bool operator()(const struct SLoadFileDesc &f1, const struct SLoadFileDesc &f2) const;
  };

  enum ECreateGameMode
  {
    E_CHOOSE_NEW_MAP = 0,
    E_DELAYED_UPDATE = 1,
  };

  bool ProcessMPCommand(const SToUICommand &cmd);

private:
  CPtr<CMapSettingsWrapper> pMapSettingsWrapper;

  CListControlWrapper<SUIMapInfo, std::string> mapsList;

  NInput::CCommandRegistrator commandMsgs;
  CInterfaceMPCreateGame() : CInterfaceMultiplayerScreen("InterMission") {}
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;

  void OnSelectionChanged();
  void PrepareMapsList();
  void CreateGame();

public:
  bool STDCALL Init() override;

  void Create(/* ECreateGameMode */ int eMode);
};

class CICMultyplayerCreateGame : public CInterfaceCommandBase<CInterfaceMPCreateGame, MISSION_INTERFACE_MULTYPLAYER_CREATEGAME>
{
  OBJECT_NORMAL_METHODS(CICMultyplayerCreateGame);
  int /* ECreateGameMode */ eType;

  void PreCreate(IMainLoop *pML) override { pML->ResetStack(); }

  void PostCreate(IMainLoop *pML, CInterfaceMPCreateGame *pIMM) override
  {
    pIMM->Create(eType);
    pML->PushInterface(pIMM);
  }

public:
  CICMultyplayerCreateGame() : eType(0) {}

  void STDCALL Configure(const char *pszConfig) override { if (pszConfig) eType = NStr::ToInt(pszConfig); }
};

class CICGameSpyCreateHost : public CInterfaceCommandBase<CInterfaceMPCreateGame, MISSION_INTERFACE_MULTYPLAYER_CREATEGAME>
{
  OBJECT_NORMAL_METHODS(CICGameSpyCreateHost);
  int nPort;

  void PreCreate(IMainLoop *pML) override
  {
    GetSingleton<IMPToUICommandManager>()->SetConnectionType(EMCT_GAMESPY);
    pML->ResetStack();
  }

  void PostCreate(IMainLoop *pML, CInterfaceMPCreateGame *pIMM) override
  {
    GetSingleton<ITransceiver>()->CreateServer();
    pIMM->Create(0);
    pML->PushInterface(pIMM);
  }

public:
  CICGameSpyCreateHost() : nPort(0) {}

  void STDCALL Configure(const char *pszConfig) override
  {
    if (!pszConfig) return;

    std::vector<std::string> szParams;
    NStr::SplitString(pszConfig, szParams, '"');

    NI_ASSERT_T(szParams.size() > 0, "Wrong gamespy commandline params");

    nPort = NStr::ToInt(szParams[0]);

    if (szParams.size() > 1) SetGlobalVar("Multiplayer.GameSpyPassword", szParams[1].c_str());
    else RemoveGlobalVar("Multiplayer.GameSpyPassword");

    GetSingleton<IMainLoop>()->Command(MAIN_COMMAND_CHANGE_TRANSCEIVER, NStr::Format("%d %d", MAIN_MP_TRANSCEIVER, EMCT_GAMESPY));
  }
};

#endif // __INTERFACEMPCREATEGAME_H__