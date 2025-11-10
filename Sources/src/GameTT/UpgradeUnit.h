#ifndef __IM_UPGRADE_UNIT_H__
#define __IM_UPGRADE_UNIT_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

class CInterfaceUpgradeUnit : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceUpgradeUnit);
  // input
  NInput::CCommandRegistrator commandMsgs;
  bool bToChapter;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceUpgradeUnit() override;
  CInterfaceUpgradeUnit() : CInterfaceInterMission("InterMission") {}

  void DefaultUpgrades();

public:
  void SetToChapter(const bool _bToChapter) { bToChapter = _bToChapter; }
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
  void STDCALL OnGetFocus(bool bFocus) override;
};

class CICUpgradeUnit : public CInterfaceCommandBase<CInterfaceUpgradeUnit, MISSION_INTERFACE_UPGRADE_UNIT>
{
  OBJECT_NORMAL_METHODS(CICUpgradeUnit);
  bool bToChapter;// if true, then to chapter, later to

  void PreCreate(IMainLoop *pML) override { if (bToChapter) pML->ResetStack(); }

  void PostCreate(IMainLoop *pML, CInterfaceUpgradeUnit *pInterface) override
  {
    pInterface->SetToChapter(bToChapter);
    pML->PushInterface(pInterface);
  }

  //
  CICUpgradeUnit() : bToChapter(false) {}

public:
  void STDCALL Configure(const char *pszConfig) override { if (pszConfig) { bToChapter = NStr::ToInt(pszConfig); } }

};

#endif		// __IM_UPGRADE_UNIT_H__