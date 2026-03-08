#ifndef __IM_NEXT_CHAPTER_H__
#define __IM_NEXT_CHAPTER_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

class CInterfaceNextChapter : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceNextChapter);
  // input
  NInput::CCommandRegistrator commandMsgs;
  bool bAllowStay;// allow stay in surrent chapter
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceNextChapter() override;
  CInterfaceNextChapter();

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
};

class CICNextChapter : public CInterfaceCommandBase<IInterfaceBase, MISSION_INTERFACE_NEXT_CHAPTER>
{
  OBJECT_NORMAL_METHODS(CICNextChapter);

  // virtual void PreCreate( IMainLoop *pML ) {}
  void PostCreate(IMainLoop *pML, IInterfaceBase *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICNextChapter() {}
};

#endif		// __IM_NEXT_CHAPTER_H__