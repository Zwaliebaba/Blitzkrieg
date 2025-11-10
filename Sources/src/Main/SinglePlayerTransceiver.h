#ifndef __SINGLEPLAYERTRANSCEIVER_H__
#define __SINGLEPLAYERTRANSCEIVER_H__

#pragma once

#include "Transceiver.h"
#include "../AILogic/AILogic.h"

interface ICommandsHistory;

class CSinglePlayerTransceiver : public CTRefCount<ITransceiver>
{
  OBJECT_SERVICE_METHODS(CSinglePlayerTransceiver);
  DECLARE_SERIALIZE;
  //
  CPtr<IAILogic> pAILogic;// shortcut to AI logic

  // team history
  CPtr<ICommandsHistory> pCmdsHistory;
  // common segment number - for command history
  long nCommonSegment;
  bool bHistoryPlaying;

public:
  void STDCALL Init(ISingleton *pSingleton, int nMultiplayerType) override;
  void STDCALL Done() override {}
  void STDCALL PreMissionInit() override;
  // set latency for smooth net playing
  void SetLatency(int nSegments) override {}
  // perform segments for AI
  void STDCALL DoSegments() override;
  // register group of units to AI
  int STDCALL CommandRegisterGroup(IRefCount **pUnitsBuffer, int nLen) override;
  // unregistered group
  void STDCALL CommandUnregisterGroup(WORD wGroup) override;
  // send command to group of units
  void STDCALL CommandGroupCommand(const SAIUnitCmd *pCommand, WORD wGroup, bool bPlaceInQueue) override;
  // set single command to call planes, reinforcements, etc. 
  int STDCALL CommandUnitCommand(const struct SAIUnitCmd *pCommand) override;
  // show different areas... this command actually has been doing only on client, but it needs group to be registered
  void STDCALL CommandShowAreas(int nGroupID, int nAreaType, bool bShow) override;
  // process pCommand received in the current segment
  void STDCALL AddCommandToSend(IAILogicCommand *pCommand) override;

  // number of players currently playing
  int STDCALL GetNumberOfPlayers() const override { return 1; }

  bool STDCALL JoinToServer(const char *pszIPAddress, const int nPort, bool bPasswordRequired, const char *pszPassword) override { return false; }
  void STDCALL CreateServer() override {}
  void STDCALL InitByCreateServersList() override {}

  // client commands
  void STDCALL CommandClientTogglePause() override;
  void STDCALL CommandClientSpeed(int nChange) override;
  void STDCALL CommandClientDropPlayer(const WORD *pszPlayerNick) override {}

  void STDCALL CommandTimeOut(const bool bSet) override {}

  NTimer::STime STDCALL GetMultiplayerTime() override { return 0; }

  void STDCALL LoadAllGameParameters() override;

  void STDCALL SetTotalOutOfSync() override {}

  void STDCALL GameFinished() override {}
};

#endif // __SINGLEPLAYERTRANSCEIVER_H__