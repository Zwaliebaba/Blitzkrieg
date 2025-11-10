#ifndef __INTERFACEMPADDRESSBOOK_H__
#define __INTERFACEMPADDRESSBOOK_H__

#pragma once


#include "../Common/InterfaceScreenBase.h"
#include "../Input/InputHelper.h"
#include "iMission.h"
#include "MapSettingsWrapper.h"
#include "InterMission.h"

class CInterfaceMPAddressBook : public CInterfaceMultiplayerScreen
{
  OBJECT_NORMAL_METHODS(CInterfaceMPAddressBook);
  //
  NInput::CCommandRegistrator msgs;
  using CServersList = std::hash_set<std::string>;

  CServersList szServers;
  bool bChanged;

  IUIDialog *pDialogAskAddress;
  IUIDialog *pDialogWaitForConnection;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  ~CInterfaceMPAddressBook() override {}
  bool ProcessMPCommand(const SToUICommand &cmd);

  void CheckEnableButtons();
  std::string GetServer();
  void AddServer(const std::string &szServer);
  void AddServerInternal(const std::string &szServer);
  void DeleteServer();

  void Serialize(bool bRead);
  void SaveServersList();
  void InitServersList();

protected:
  CInterfaceMPAddressBook() : CInterfaceMultiplayerScreen("InterMission"), pDialogAskAddress(nullptr), pDialogWaitForConnection(nullptr) {}

public:
  bool STDCALL Init() override;
  void STDCALL Done() override;
  void STDCALL StartInterface() override;
  void STDCALL OnGetFocus(bool bFocus) override;
};

class CICMPAddressBook : public CInterfaceCommandBase<CInterfaceMPAddressBook, MISSION_INTERFACE_ADDRESS_BOOK>
{
  OBJECT_NORMAL_METHODS(CICMPAddressBook);
  void PreCreate(IMainLoop *pML) override { pML->ResetStack(); }

  void PostCreate(IMainLoop *pML, CInterfaceMPAddressBook *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICMPAddressBook() {}

public:
  void STDCALL Configure(const char *pszConfig) override {}
};

#endif // __INTERFACEMPADDRESSBOOK_H__