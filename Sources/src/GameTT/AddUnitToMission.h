#ifndef __IM_ADD_UNIT_TO_MISSION_H__
#define __IM_ADD_UNIT_TO_MISSION_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

class CInterfaceAddUnitToMission : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceAddUnitToMission);
  // input
  NInput::CCommandRegistrator commandMsgs;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceAddUnitToMission() override;
  CInterfaceAddUnitToMission();

  void UpdateUnitsList();
  void SelectItem();
  void DisplaySlotsFromST();
  void EnableItem(IUIContainer *pItem, bool bEnable);

  // array of arrays, size number of AI classes
  // the internal array contains the indexes of units added to the mission, or -1 if the unit is not taken.
  static std::vector<std::vector<int>> m_missionSlots;

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;

  // this function reads slots from the map and fills them with the first available units of this class
  // information is immediately loaded into ScenarioTracker
  static bool AddDefaultSlotsToST();
};

class CICAddUnitToMission : public CInterfaceCommandBase<IInterfaceBase, MISSION_INTERFACE_ADD_UNIT_TO_MISSION>
{
  OBJECT_NORMAL_METHODS(CICAddUnitToMission);

  void PreCreate(IMainLoop *pML) override {}
  void PostCreate(IMainLoop *pML, IInterfaceBase *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICAddUnitToMission() {}
};

#endif		// __IM_ADD_UNIT_TO_MISSION_H__