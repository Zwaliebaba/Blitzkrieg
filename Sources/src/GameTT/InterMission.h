#ifndef __INTERMISSION_H__
#define __INTERMISSION_H__

#pragma once

#include "../Common/InterfaceScreenBase.h"
#include "../Input/InputHelper.h"


class CInterfaceInterMission : public CInterfaceScreenBase
{
  //
  void STDCALL DrawAdd() override;

protected:
  // input
  NInput::CCommandRegistrator intermissionMsgs;

  bool STDCALL StepLocal(bool bAppActive) override;
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  CInterfaceInterMission(const std::string &szInterfaceType) : CInterfaceScreenBase(szInterfaceType) {}

public:
  bool STDCALL Init() override;
};

interface IMPToUICommandManager;

class CInterfaceMultiplayerScreen : public CInterfaceInterMission
{
protected:
  CPtr<IMPToUICommandManager> pCommandManager;// singleton shortcut

  CInterfaceMultiplayerScreen(const std::string &szInterfaceType) : CInterfaceInterMission(szInterfaceType) {}
  bool STDCALL StepLocal(bool bAppActive) override;

public:
  bool STDCALL Init() override;

};

#endif // __INTERMISSION_H__