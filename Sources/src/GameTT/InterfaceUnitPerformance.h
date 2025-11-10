#ifndef __INTERFACEUNITPerformance_H__
#define __INTERFACEUNITPerformance_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"
class CAfterMissionPopups;

class CInterfaceUnitPerformance : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceUnitPerformance);
  DECLARE_SERIALIZE;
  // input
  NInput::CCommandRegistrator commandMsgs;
  // CPtr<CAfterMissionPopups> pPopups;
  // bool bDisableGetFocus;

  int nPlayerUnits;
  int nTotalNumUnits;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  ~CInterfaceUnitPerformance() override {}

  void PrepairShortcutBar();
  const char *GetUnitNameByWindowID(int nWindowID);

protected:
  CInterfaceUnitPerformance() : CInterfaceInterMission("InterMission"),
                                /* bDisableGetFocus( true ), */ nPlayerUnits(0), nTotalNumUnits(0) {}

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
  void STDCALL OnGetFocus(bool bFocus) override;
};

class CICUnitPerformance : public CInterfaceCommandBase<CInterfaceUnitPerformance, MISSION_INTERFACE_UNIT_PERFORMANCE>
{
  OBJECT_NORMAL_METHODS(CICUnitPerformance);

  bool bDisableChange;

  void PreCreate(IMainLoop *pML) override
  {
    // pML->ResetStack();
  }

  void PostCreate(IMainLoop *pML, CInterfaceUnitPerformance *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICUnitPerformance() {}

public:
  void STDCALL Configure(const char *pszConfig) override {}
};

#endif // __INTERFACEUNITPerformance_H__