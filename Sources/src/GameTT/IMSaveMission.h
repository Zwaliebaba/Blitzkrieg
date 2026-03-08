#ifndef __IMSAVEMISSION_H__
#define __IMSAVEMISSION_H__

#pragma once

#include "BaseList.h"

class CInterfaceIMSaveMission : public CInterfaceBaseList
{
  OBJECT_NORMAL_METHODS(CInterfaceIMSaveMission);
  bool bClosed;
  std::string szProspectiveFileName;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceIMSaveMission() override;

  std::unordered_map<int/* nUserData */, std::string/* file name */> szSaves;

  void OnSelectionChanged();
  void Save();

protected:
  CInterfaceIMSaveMission() {}

  // functions I overload
  bool FillListItem(IUIListRow *pRow, const std::string &szFullFileName, bool *pSelectedItem = nullptr) override;// fill the current line in the list
  bool OnOk(const std::string &szFullFileName) override;// the user has selected a file, let's process the selection

  bool IsIgnoreSelection() const override { return true; }// user may not select, but enter to edit box
  bool OnOk() override;
  void STDCALL OnGetFocus(bool bFocus) override;

public:
  bool STDCALL Init() override;
  void STDCALL StartInterface() override;
};

class CICIMSaveMission : public CInterfaceCommandBase<CInterfaceIMSaveMission, MISSION_INTERFACE_IM_SAVE_MISSION>
{
  OBJECT_NORMAL_METHODS(CICIMSaveMission);

  void PostCreate(IMainLoop *pML, CInterfaceIMSaveMission *pILM) override;
  //
  CICIMSaveMission() {}
};


#endif // __IMSAVEMISSION_H__