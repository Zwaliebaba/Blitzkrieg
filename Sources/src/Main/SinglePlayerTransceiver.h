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
  void Init(ISingleton *pSingleton, int nMultiplayerType) override;
  void Done() override {}
  void PreMissionInit() override;
  // set latency for smooth net playing
  void SetLatency(int nSegments) override {}
  // perform segments for AI
  void DoSegments() override;
  // register group of units to AI
  int CommandRegisterGroup(IRefCount **pUnitsBuffer, int nLen) override;
  // unregistered group
  void CommandUnregisterGroup(WORD wGroup) override;
  // send command to group of units
  void CommandGroupCommand(const SAIUnitCmd *pCommand, WORD wGroup, bool bPlaceInQueue) override;
  // set single command to call planes, reinforcements, etc. 
  int CommandUnitCommand(const struct SAIUnitCmd *pCommand) override;
  // show different areas... this command actually has been doing only on client, but it needs group to be registered
  void CommandShowAreas(int nGroupID, int nAreaType, bool bShow) override;
  // process pCommand received in the current segment
  void AddCommandToSend(IAILogicCommand *pCommand) override;

  // number of players currently playing
  int GetNumberOfPlayers() const override { return 1; }

  bool JoinToServer(const char *pszIPAddress, const int nPort, bool bPasswordRequired, const char *pszPassword) override { return false; }
  void CreateServer() override {}
  void InitByCreateServersList() override {}

  // client commands
  void CommandClientTogglePause() override;
  void CommandClientSpeed(int nChange) override;
  void CommandClientDropPlayer(const wchar_t *pszPlayerNick) override {}

  void CommandTimeOut(const bool bSet) override {}

  NTimer::STime GetMultiplayerTime() override { return 0; }

  void LoadAllGameParameters() override;

  void SetTotalOutOfSync() override {}

  void GameFinished() override {}
};

#endif // __SINGLEPLAYERTRANSCEIVER_H__