#ifndef __IM_CUTSCENE_LIST_H__
#define __IM_CUTSCENE_LIST_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InterMission.h"
#include "iMission.h"

class CInterfaceCutsceneList : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceCutsceneList);
  //
  NInput::CCommandRegistrator commandMsgs;
  std::vector<std::string> cutscenesList;// list of video files
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceCutsceneList() override;
  CInterfaceCutsceneList() : CInterfaceInterMission("InterMission") {}

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
};

class CCutsceneList : public CInterfaceCommandBase<CInterfaceCutsceneList, MISSION_INTERFACE_CUTSCENE_LIST>
{
  OBJECT_NORMAL_METHODS(CCutsceneList);

  void PostCreate(IMainLoop *pML, CInterfaceCutsceneList *pILM) override;
  //
  CCutsceneList() {}
};

#endif // __IM_CUTSCENE_LIST_H__