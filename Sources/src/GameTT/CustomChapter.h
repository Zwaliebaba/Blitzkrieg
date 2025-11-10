#ifndef __IM_CUSTOM_CHAPTER_H__
#define __IM_CUSTOM_CHAPTER_H__

#pragma once

#include "CustomList.h"

class CInterfaceCustomChapter : public CInterfaceCustomList
{
  OBJECT_NORMAL_METHODS(CInterfaceCustomChapter);
  // disable explicit destruction
  ~CInterfaceCustomChapter() override;

protected:
  CInterfaceCustomChapter() {}

  // functions I overload
  bool FillListItem(IUIListRow *pRow, const std::string &szFullFileName, bool *pSelectedItem = nullptr) override;// fill the current line in the list
  bool OnOk(const std::string &szFullFileName) override;// the user has selected a file, let's process the selection
public:
  bool STDCALL Init() override;
};

class CICCustomChapter : public CInterfaceCommandBase<CInterfaceCustomChapter, MISSION_INTERFACE_CUSTOM_CHAPTER>
{
  OBJECT_NORMAL_METHODS(CICCustomChapter);

  // virtual void PreCreate( IMainLoop *pML ) { pML->ResetStack(); 
  void PostCreate(IMainLoop *pML, CInterfaceCustomChapter *pInterface) override { pML->PushInterface(pInterface); }
  //
  CICCustomChapter() {}
};

#endif		// __IM_CUSTOM_CHAPTER_H__