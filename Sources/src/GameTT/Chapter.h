#ifndef __CHAPTER_H__
#define __CHAPTER_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "InterMission.h"
#include "iMission.h"

struct SChapterStats;
class CAfterMissionPopups;

class CInterfaceChapter : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceChapter);
  DECLARE_SERIALIZE;
  // input
  NInput::CCommandRegistrator commandMsgs;
  CPtr<CAfterMissionPopups> pPopups;
  int nSelected;// current selected mission

  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceChapter() override;
  CInterfaceChapter() : CInterfaceInterMission("InterMission"), nSelected(-1) {}
  //
  void IncrementChapterVisited();

  std::vector<int> missionIndeces;
  void SetMissionDescription(int nSelected);
  void InitWindow();

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;

  static const SChapterStats *ReadChapterStats();
};

class CICChapter : public CInterfaceCommandBase<IInterfaceBase, MISSION_INTERFACE_CHAPTER>
{
  OBJECT_NORMAL_METHODS(CICChapter);

  void PreCreate(IMainLoop *pML) override { pML->ResetStack(); }
  void PostCreate(IMainLoop *pML, IInterfaceBase *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICChapter() {}
};

#endif	// __CHAPTER_H__