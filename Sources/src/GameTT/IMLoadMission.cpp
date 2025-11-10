#include "StdAfx.h"

#include "CommonId.h"
#include "IMLoadMission.h"
#include "../Main/iMainCommands.h"
#include "../Main/ScenarioTracker.h"

void CICIMLoadMission::PostCreate(IMainLoop *pML, CInterfaceIMLoadMission *pILM) { pML->PushInterface(pILM); }

CInterfaceIMLoadMission::~CInterfaceIMLoadMission() {}

bool CInterfaceIMLoadMission::Init()
{
  // initialize names
  fileMasks.clear();
  fileMasks.push_back("*.sav");
  szTopDir = GetSingleton<IMainLoop>()->GetBaseDir();
  const std::string szModname = GetSingleton<IUserProfile>()->GetMOD();
  if (!szModname.empty())
  {
    szTopDir += "mods\\";
    szTopDir += szModname;
  }
  szTopDir += "saves\\";
  szCurrentDir = szTopDir;
  szInterfaceName = "ui\\Lists\\IMLoadMission";
  nSortType = E_SORT_BY_TIME;
  nFirstSortColumn = 1;
  //
  CInterfaceBaseList::Init();
  //
  return true;
}

bool CInterfaceIMLoadMission::FillListItem(IUIListRow *pRow, const std::string &szFullFileName, bool *pSelectedItem)
{
  IUIElement *pElement = pRow->GetElement(1);
  if (!pElement) return false;

  std::string szVal = GetFileChangeTimeString(szFullFileName.c_str());
  pElement->SetWindowText(0, NStr::ToUnicode(szVal).c_str());
  return true;
}

bool CInterfaceIMLoadMission::OnOk(const std::string &szFullFileName)
{
  IMainLoop *pML = GetSingleton<IMainLoop>();
  std::string szShortName = szFullFileName.substr(szTopDir.size());
  CloseInterface();
  pML->Command(MAIN_COMMAND_LOAD, szShortName.c_str());
  pML->Command(MAIN_COMMAND_CMD, NStr::Format("%d", CMD_GAME_UNPAUSE_MENU));// let's take a break
  return true;
}

bool CInterfaceIMLoadMission::ProcessMessage(const SGameMessage &msg)
{
  if (CInterfaceBaseList::ProcessMessage(msg)) return true;

  switch (msg.nEventID)
  {
    /* case IMC_SELECTION_CHANGED:
           */

    case IMC_CANCEL:
    {
      CloseInterface();
      /* pML->Command( MAIN_COMMAND_CMD, NStr::Format("%d", CMD_GAME_UNPAUSE_MENU) );	 */
      return true;
    }

      /* case IMC_OK:
             */
  }

  //
  return false;
}