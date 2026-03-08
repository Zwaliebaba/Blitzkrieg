#ifndef __INTERFACENEWDEPOTUPGRADES_H__
#define __INTERFACENEWDEPOTUPGRADES_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

class CInterfaceNewDepotUpgrades : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceNewDepotUpgrades);
  // input
  NInput::CCommandRegistrator commandMsgs;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceNewDepotUpgrades() override;
  CInterfaceNewDepotUpgrades();

public:
  void STDCALL StartInterface() override;
};

class CICNewDepotUpgrades : public CInterfaceCommandBase<IInterfaceBase, MISSION_INTERFACE_NEW_DEPOTUPGRADES>
{
  OBJECT_NORMAL_METHODS(CICNewDepotUpgrades);

  void PostCreate(IMainLoop *pML, IInterfaceBase *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICNewDepotUpgrades() {}
};

#endif // __INTERFACENEWDEPOTUPGRADES_H__