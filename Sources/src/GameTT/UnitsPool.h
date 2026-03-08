#ifndef __IM_UNITS_POOL_H__
#define __IM_UNITS_POOL_H__

#pragma once

#include "InterMission.h"
#include "iMission.h"

class CInterfaceUnitsPool : public CInterfaceInterMission
{
  OBJECT_NORMAL_METHODS(CInterfaceUnitsPool);
  // input
  NInput::CCommandRegistrator commandMsgs;
  std::vector<std::vector<int>> units;
  std::vector<std::vector<int>> depot;
  //
  bool STDCALL ProcessMessage(const SGameMessage &msg) override;
  // disable explicit destruction
  ~CInterfaceUnitsPool() override;
  CInterfaceUnitsPool() : CInterfaceInterMission("InterMission") {}

public:
  bool STDCALL Init() override;
  void Create(int nNewUnits);
};

class CICUnitsPool : public CInterfaceCommandBase<CInterfaceUnitsPool, MISSION_INTERFACE_UNITS_POOL>
{
  OBJECT_NORMAL_METHODS(CICUnitsPool);

  int nNewUnits;

  void PreCreate(IMainLoop *pML) override {}
  void PostCreate(IMainLoop *pML, CInterfaceUnitsPool *pIUP) override;
  //
  CICUnitsPool() {}

public:
  void STDCALL Configure(const char *pszConfig) override;
};

#endif		// __IM_UNITS_POOL_H__