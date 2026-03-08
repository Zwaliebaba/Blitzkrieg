#ifndef __IM_LOAD_MISSION_H__
#define __IM_LOAD_MISSION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseList.h"

class CInterfaceIMLoadMission : public CInterfaceBaseList
{
  OBJECT_NORMAL_METHODS(CInterfaceIMLoadMission);
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceIMLoadMission() override;

protected:
  CInterfaceIMLoadMission() {}

  // functions I overload
  bool FillListItem(IUIListRow *pRow, const std::string &szFullFileName, bool *pSelectedItem = nullptr) override;// fill the current line in the list
  bool OnOk(const std::string &szFullFileName) override;// the user has selected a file, let's process the selection
public:
  bool STDCALL Init() override;
};

class CICIMLoadMission : public CInterfaceCommandBase<CInterfaceIMLoadMission, MISSION_INTERFACE_IM_LOAD_MISSION>
{
  OBJECT_NORMAL_METHODS(CICIMLoadMission);

  void PostCreate(IMainLoop *pML, CInterfaceIMLoadMission *pILM) override;
  //
  CICIMLoadMission() {}
};

#endif // __IM_LOAD_MISSION_H__