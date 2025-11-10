#ifndef __CAMPAIGN_H__
#define __CAMPAIGN_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

struct SCampaignStats;

class CInterfaceCampaign : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceCampaign);
  DECLARE_SERIALIZE;
  //
  using CChapterNameToButtonIndex = std::unordered_map<std::string, int>;
  CChapterNameToButtonIndex chapterNameToButtonIndexMap;
  // input
  NInput::CCommandRegistrator commandMsgs;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceCampaign() override;
  CInterfaceCampaign() : CInterfaceInterMission("InterMission") {}
  //
  void SetDescriptionText(const struct SChapterStats *pStats);
  void OnCancel();

public:
  static void PlayCampaignMusic();
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
  static const SCampaignStats *ReadCampaignStats();
  void STDCALL OnGetFocus(bool bFocus) override;
};

class CICCampaign : public CInterfaceCommandBase<IInterfaceBase, MISSION_INTERFACE_CAMPAIGN>
{
  OBJECT_NORMAL_METHODS(CICCampaign);

  void PreCreate(IMainLoop *pML) override { pML->ResetStack(); }
  void PostCreate(IMainLoop *pML, IInterfaceBase *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICCampaign() {}
};

#endif		// __CAMPAIGN_H__