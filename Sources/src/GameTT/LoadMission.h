#ifndef __LOAD_MISSION_H__
#define __LOAD_MISSION_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "../Common/InterfaceScreenBase.h"
#include "../Input/InputHelper.h"
#include "iMission.h"

class CInterfaceLoadMission : public CInterfaceScreenBase
{
  OBJECT_NORMAL_METHODS(CInterfaceLoadMission);
  //
  std::vector<std::string> szSaves;
  // input
  NInput::CCommandRegistrator loadmissionMsgs;
  //
  bool ProcessMessage(const SGameMessage &msg) override;
  bool StepLocal(bool bAppActive) override;
  void DrawAdd() override;
  // disable explicit destruction
  ~CInterfaceLoadMission() override;

protected:
  CInterfaceLoadMission() : CInterfaceScreenBase("Current") {}

public:
  bool Init() override;
  void StartInterface() override;
};

class CICLoadMission : public CInterfaceCommandBase<CInterfaceLoadMission, MISSION_INTERFACE_LOAD_MISSION>
{
  OBJECT_NORMAL_METHODS(CICLoadMission);

  void PostCreate(IMainLoop *pML, CInterfaceLoadMission *pILM) override;
  //
  CICLoadMission() {}
};

#endif // __LOAD_MISSION_H__