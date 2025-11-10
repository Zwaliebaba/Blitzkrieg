#ifndef __IM_TUTORIAL_H__
#define __IM_TUTORIAL_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

class CInterfaceIMTutorial : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceIMTutorial);
  // input
  NInput::CCommandRegistrator commandMsgs;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceIMTutorial() override;
  CInterfaceIMTutorial();

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
};

class CICIMTutorial : public CInterfaceCommandBase<IInterfaceBase, MISSION_INTERFACE_IM_TUTORIAL>
{
  OBJECT_NORMAL_METHODS(CICIMTutorial);

  // virtual void PreCreate( IMainLoop *pML ) {}
  void PostCreate(IMainLoop *pML, IInterfaceBase *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICIMTutorial() {}
};

#endif		// __IM_TUTORIAL_H__