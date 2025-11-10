#ifndef __IM_CUSTOM_CAMPAIGN_H__
#define __IM_CUSTOM_CAMPAIGN_H__

#pragma once

#include "CustomList.h"

class CInterfaceCustomCampaign : public CInterfaceCustomList
{
  OBJECT_NORMAL_METHODS(CInterfaceCustomCampaign);
  // disable explicit destruction
  ~CInterfaceCustomCampaign() override;

protected:
  CInterfaceCustomCampaign() {}

  // functions I overload
  bool FillListItem(IUIListRow *pRow, const std::string &szFullFileName, bool *pSelectedItem = nullptr) override;// fill the current line in the list
  bool OnOk(const std::string &szFullFileName) override;// the user has selected a file, let's process the selection
public:
  bool STDCALL Init() override;
};

class CICCustomCampaign : public CInterfaceCommandBase<CInterfaceCustomCampaign, MISSION_INTERFACE_CUSTOM_CAMPAIGN>
{
  OBJECT_NORMAL_METHODS(CICCustomCampaign);

  // virtual void PreCreate( IMainLoop *pML ) { pML->ResetStack(); 
  void PostCreate(IMainLoop *pML, CInterfaceCustomCampaign *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICCustomCampaign() {}
};

#endif		// __IM_CUSTOM_CAMPAIGN_H__