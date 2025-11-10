#include "StdAfx.h"

#include "SaveReplay.h"

#include "CommonId.h"
#include "../Main/CommandsHistoryInterface.h"
#include "../UI/UIMessages.h"
#include "../Main/ScenarioTracker.h"

enum EControls
{
  E_REPLAY_EDIT_BOX = 2000,
  E_OK_BUTTON = 10002,
};

static constexpr NInput::SRegisterCommandEntry commands[] =
{
    {"inter_ok", IMC_OK},
    {"inter_cancel", IMC_CANCEL},
    {nullptr, 0}
};

CInterfaceSaveReplay::~CInterfaceSaveReplay() {}

bool CInterfaceSaveReplay::Init()
{
  CInterfaceInterMission::Init();
  commandMsgs.Init(pInput, commands);

  return true;
}

void CInterfaceSaveReplay::StartInterface()
{
  CInterfaceInterMission::StartInterface();
  pUIScreen = CreateObject<IUIScreen>(UI_SCREEN);
  pUIScreen->Load("ui\\Popup\\SaveReplay");
  pUIScreen->Reposition(pGFX->GetScreenRect());

  auto pDialog = checked_cast<IUIDialog *>(pUIScreen->GetChildByID(1000));
  IUIElement *pSaveReplayEditBox = pDialog->GetChildByID(2000);
  pSaveReplayEditBox->SetFocus(true);

  pScene->AddUIScreen(pUIScreen);
  CheckEnableOk();
}

void CInterfaceSaveReplay::CheckEnableOk() const
{
  IUIElement *pEdit = pUIScreen->GetChildByID(E_REPLAY_EDIT_BOX);
  const std::wstring szName = pEdit->GetWindowText(0);
  IUIElement *pButtonOK = pUIScreen->GetChildByID(E_OK_BUTTON);
  pButtonOK->EnableWindow(!szName.empty());
}

void CInterfaceSaveReplay::OnGetFocus(bool bFocus)
{
  CInterfaceInterMission::OnGetFocus(bFocus);
  /* if ( bFocus && GetGlobalVar( "SaveGame.OkOverrite", 0 ) )
   */
}

void CInterfaceSaveReplay::OnSave()
{
  // let's record replay
  GetSingleton<ICommandsHistory>()->Save(szSaveReplayFile.c_str());

  SetGlobalVar("ReplaySaved", 1);
  CloseInterface();
}

bool CInterfaceSaveReplay::ProcessMessage(const SGameMessage &msg)
{
  if (CInterfaceInterMission::ProcessMessage(msg)) return true;

  switch (msg.nEventID)
  {
    case UI_NOTIFY_EDIT_BOX_TEXT_CHANGED: { CheckEnableOk(); }
      return true;
    case IMC_CANCEL:
      SetGlobalVar("ReplaySaved", 0);
      CloseInterface();
      return true;

    case IMC_OK:
    {
      IMainLoop *pML = GetSingleton<IMainLoop>();
      // get the name for saving replay file
      szSaveReplayFile = pML->GetBaseDir();
      const std::string szModname = GetSingleton<IUserProfile>()->GetMOD();
      if (!szModname.empty())
      {
        szSaveReplayFile += "mods\\";
        szSaveReplayFile += szModname;
      }
      szSaveReplayFile += "replays\\";
      auto pDialog = checked_cast<IUIDialog *>(pUIScreen->GetChildByID(1000));
      IUIElement *pSaveReplayEditBox = pDialog->GetChildByID(2000);
      szSaveReplayFile += NStr::ToAscii(pSaveReplayEditBox->GetWindowText(0));
      szSaveReplayFile += ".rpl";

      /* if ( NFile::IsFileExist( szFileName.c_str() ) )
       */
      OnSave();
      return true;
    }
  }

  //
  return false;
}