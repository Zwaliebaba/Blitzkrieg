#ifndef __QUIT_MISSION_H__
#define __QUIT_MISSION_H__

#pragma once

#include "../Common/InterfaceScreenBase.h"
#include "../Input/InputHelper.h"
#include "iMission.h"

class CInterfaceQuitMission : public CInterfaceScreenBase
{
  OBJECT_NORMAL_METHODS(CInterfaceQuitMission);
  // input
  NInput::CCommandRegistrator quitmissionMsgs;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  bool STDCALL StepLocal(bool bAppActive) override;
  void STDCALL DrawAdd() override;
  // disable explicit destruction
  ~CInterfaceQuitMission() override;

protected:
  CInterfaceQuitMission() : CInterfaceScreenBase("Current") {}

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
};

class CICQuitMission : public CInterfaceCommandBase<IInterfaceBase, MISSION_INTERFACE_QUIT_MISSION>
{
  OBJECT_NORMAL_METHODS(CICQuitMission);

  void PostCreate(IMainLoop *pML, IInterfaceBase *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICQuitMission() {}
};

#endif // __QUIT_MISSION_H__