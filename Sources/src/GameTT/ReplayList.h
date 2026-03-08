#ifndef __IM_REPLAY_LIST_H__
#define __IM_REPLAY_LIST_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseList.h"

enum EReplayError
{
  ERR_NO_ERROR = 0,
  ERR_BAD_RESOURCES = 1,
  ERR_BAD_MAP = 2,
};

class CInterfaceReplayList : public CInterfaceBaseList
{
  OBJECT_NORMAL_METHODS(CInterfaceReplayList);

  bool bInstantLoad;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceReplayList() override;
  void DisplayError();

protected:
  CInterfaceReplayList() : bInstantLoad(false) {}

  // functions I overload
  bool FillListItem(IUIListRow *pRow, const std::string &szFullFileName, bool *pSelectedItem = nullptr) override;// fill the current line in the list
  bool OnOk(const std::string &szFullFileName) override;// the user has selected a file, let's process the selection
public:
  bool STDCALL Init() override;
  void SetFileName(const char *pszFileName);
  void STDCALL StartInterface() override;
};

class CReplayList : public CInterfaceCommandBase<CInterfaceReplayList, MISSION_INTERFACE_REPLAY_LIST>
{
  OBJECT_NORMAL_METHODS(CReplayList);

  std::string szFileNameToLoad;

  void PostCreate(IMainLoop *pML, CInterfaceReplayList *pILM) override;
  //
  CReplayList() {}

public:
  void STDCALL Configure(const char *pszConfig) override { if (pszConfig) szFileNameToLoad = pszConfig; }
};

#endif // __IM_REPLAY_LIST_H__