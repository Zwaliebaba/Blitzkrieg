#ifndef __INTERFACEIMMODSLIST_H__
#define __INTERFACEIMMODSLIST_H__

#pragma once

#include "BaseList.h"

class CInterfaceIMModsList : public CInterfaceBaseList
{
  OBJECT_NORMAL_METHODS(CInterfaceIMModsList);
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceIMModsList() override;

protected:
  CInterfaceIMModsList() {}

  // functions I overload
  bool FillListItem(IUIListRow *pRow, const std::string &szFullFileName, bool *pSelectedItem = nullptr) override;// fill the current line in the list
  bool OnOk(const std::string &szFullFileName) override;// the user has selected a file, let's process the selection
  void PrepareList(std::vector<std::string> *pFiles) override;

public:
  bool STDCALL Init() override;
};

class CICIMModsList : public CInterfaceCommandBase<CInterfaceIMModsList, MISSION_INTERFACE_MODS_LIST>
{
  OBJECT_NORMAL_METHODS(CICIMModsList);

  void PostCreate(IMainLoop *pML, CInterfaceIMModsList *pI) override { pML->PushInterface(pI); }
  //
  CICIMModsList() {}
};

#endif // __INTERFACEIMMODSLIST_H__