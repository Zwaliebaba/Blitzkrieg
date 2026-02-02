#ifndef __INTERFACESTARTDIALOG_H__
#define __INTERFACESTARTDIALOG_H__

#pragma once

#include "../Common/InterfaceScreenBase.h"
#include "../Input/InputHelper.h"
#include "iMission.h"
class COptionsListWrapper;

class CInterfacePlayerProfile : public CInterfaceScreenBase
{
  OBJECT_NORMAL_METHODS(CInterfacePlayerProfile);
  //
  NInput::CCommandRegistrator msgs;
  //
  CPtr<IUIButton> pButtonOK;
  CPtr<IUIButton> pButtonCancel;
  CPtr<IUIEditBox> pEdit;
  bool bFinished;// interface is closed

  CPtr<COptionsListWrapper> pOptions;
  bool bEnableCancel;

  bool ProcessMessage(const SGameMessage &msg) override;
  ~CInterfacePlayerProfile() override {}
  bool StepLocal(bool bAppActive) override;

protected:
  CInterfacePlayerProfile() : CInterfaceScreenBase("Current") {}

public:
  void OnGetFocus(bool bFocus) override;
  bool Init() override;
  void StartInterface() override;
};

class CICPlayerProfile : public CInterfaceCommandBase<IInterfaceBase, MISSION_INTERFACE_PLAYER_PROFILE>
{
  OBJECT_NORMAL_METHODS(CICPlayerProfile);

  void PostCreate(IMainLoop *pML, IInterfaceBase *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICPlayerProfile() {}
};

#endif // __INTERFACESTARTDIALOG_H__