#ifndef __SELECT_MAIN_MENU_H__
#define __SELECT_MAIN_MENU_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"
#include "UIState.h"

class CInterfaceMainMenu : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceMainMenu);
  // input
  NInput::CCommandRegistrator commandMsgs;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceMainMenu() override;
  CInterfaceMainMenu();

  CUIMainMenuState mainMenuState;
  CUINewGameState newGameState;
  CUISelectCampaignState selectCampaignState;
  CUIOptionsState optionsState;
  CUIMultiplayerState multiplayerState;
  CUICustomGameState customGameState;
  CUILoadGameState loadGameState;
  CUICreditsState creditsState;
  CUIDemoMainMenuState demoMainMenu;
  int nActiveState;
  std::vector<IUIState *> states;

public:
  bool STDCALL Init() override;
  static void PlayIntermissionSound();

  void STDCALL OnGetFocus(bool bFocus) override;

  enum EUIState
  {
    E_MAIN_MENU = 0,
    E_NEW_GAME = 1,
    E_SELECT_CAMPAIGN = 2,
    E_OPTIONS = 3,
    E_MULTIPLAYER = 4,
    E_CUSTOM_GAME = 5,
    E_LOAD_GAME = 6,
    E_CREDITS = 7,
    E_DEMOVERSION_MAIN_MENU = 8,
  };

  void SetActiveState(int nState);
  IUIScreen *GetUIScreen() { return pUIScreen; }
  void Create(int nState);
  void RefreshCursor();
};

class CICMainMenu : public CInterfaceCommandBase<CInterfaceMainMenu, MISSION_INTERFACE_MAIN_MENU>
{
  OBJECT_NORMAL_METHODS(CICMainMenu);
  DECLARE_SERIALIZE;
  //
  int nState;
  //
  int nNextIC;
  std::string szNextICConfig;

  void PreCreate(IMainLoop *pML) override { pML->ResetStack(); }
  void PostCreate(IMainLoop *pML, CInterfaceMainMenu *pIMM) override;
  //
  CICMainMenu() : nState(0), nNextIC(-1) {}

public:
  void STDCALL Configure(const char *pszConfig) override;
};

#endif		// __SELECT_MAIN_MENU_H__