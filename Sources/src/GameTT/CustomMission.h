#ifndef __IM_CUSTOM_MISSION_H__
#define __IM_CUSTOM_MISSION_H__

#pragma once

#include "CustomList.h"

class CInterfaceCustomMission : public CInterfaceCustomList
{
  OBJECT_NORMAL_METHODS(CInterfaceCustomMission);
  // disable explicit destruction
  ~CInterfaceCustomMission() override;

protected:
  CInterfaceCustomMission() {}

  // functions I overload
  bool FillListItem(IUIListRow *pRow, const std::string &szFullFileName, bool *pSelectedItem = nullptr) override;// fill the current line in the list
  bool OnOk(const std::string &szFullFileName) override;// the user has selected a file, let's process the selection
public:
  bool STDCALL Init() override;
};

class CICCustomMission : public CInterfaceCommandBase<CInterfaceCustomMission, MISSION_INTERFACE_CUSTOM_MISSION>
{
  OBJECT_NORMAL_METHODS(CICCustomMission);

  // virtual void PreCreate( IMainLoop *pML ) { pML->ResetStack(); 
  void PostCreate(IMainLoop *pML, CInterfaceCustomMission *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICCustomMission() {}
};

#endif		// __IM_CUSTOM_MISSION_H__