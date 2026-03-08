#pragma once

#include "InterMission.h"
#include "iMission.h"
#include "MultiplayerCommandManager.h"
#include "ListControlWrapper.h"
#include "MapSettingsWrapper.h"
#include "UIMapINfo.h"

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
