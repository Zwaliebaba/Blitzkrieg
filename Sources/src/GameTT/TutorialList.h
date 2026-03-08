#ifndef __IM_TUTORIAL_LIST_H__
#define __IM_TUTORIAL_LIST_H__

#pragma once

#include "CustomList.h"

class CInterfaceTutorialList : public CInterfaceCustomList
{
  OBJECT_NORMAL_METHODS(CInterfaceTutorialList);
  //
  // disable explicit destruction
  ~CInterfaceTutorialList() override {}

protected:
  CInterfaceTutorialList() {}

  // functions I overload
  bool FillListItem(IUIListRow *pRow, const std::string &szFullFileName, bool *pSelectedItem = nullptr) override;// fill the current line in the list
  bool OnOk(const std::string &szFullFileName) override;// the user has selected a file, let's process the selection
public:
  bool STDCALL Init() override;
};

class CICTutorialList : public CInterfaceCommandBase<CInterfaceTutorialList, MISSION_INTERFACE_TUTORIAL_LIST>
{
  OBJECT_NORMAL_METHODS(CICTutorialList);

  // virtual void PreCreate( IMainLoop *pML ) { pML->ResetStack(); 
  void PostCreate(IMainLoop *pML, CInterfaceTutorialList *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICTutorialList() {}
};

#endif		// __IM_TUTORIAL_LIST_H__