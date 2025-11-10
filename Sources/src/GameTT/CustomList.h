#ifndef __CUSTOM_LIST_H__
#define __CUSTOM_LIST_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "BaseList.h"

class CInterfaceCustomList : public CInterfaceBaseList
{
  OBJECT_NORMAL_METHODS(CInterfaceCustomList);
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  void FillListFromCurrentDir() override;

protected:
  std::string szCollectorName;

  CInterfaceCustomList() {}
  ~CInterfaceCustomList() override;

};

#endif // __CUSTOM_LIST_H__