#ifndef __MISSION_H__
#define __MISSION_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

struct SMissionStats;

class CInterfaceAboutMission : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceAboutMission);
  // input
  NInput::CCommandRegistrator commandMsgs;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceAboutMission() override;
  CInterfaceAboutMission() : CInterfaceInterMission("InterMission"), m_nActiveObjective(-1) {}
  //
  const SMissionStats *ReadMissionStats();
  void UpdateActiveObjectiveFlag(bool bShow);
  void ShowActiveObjective(bool bShow);

  int m_nActiveObjective;

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
};

class CICAboutMission : public CInterfaceCommandBase<IInterfaceBase, MISSION_INTERFACE_ABOUT_MISSION>
{
  OBJECT_NORMAL_METHODS(CICAboutMission);

  void PreCreate(IMainLoop *pML) override { pML->ResetStack(); }
  void PostCreate(IMainLoop *pML, IInterfaceBase *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICAboutMission() {}
};

#endif		// __MISSION_H__