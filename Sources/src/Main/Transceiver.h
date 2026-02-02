#ifndef __TRANSCEIVER_H__
#define __TRANSCEIVER_H__

#pragma once

#include "../Main/iMainClassIDs.h"

interface ITransceiver : IRefCount
{
  enum { tidTypeID = MAIN_TRANSCEIVER };

  //
  virtual void Init(ISingleton *pSingleton, int nMultiplayerType) = 0;
  virtual void Done() = 0;
  virtual void PreMissionInit() = 0;
  // set latency for smooth net playing
  virtual void SetLatency(int nSegments) = 0;
  // perform segments for AI
  virtual void DoSegments() = 0;
  // register group of units to AI
  virtual int CommandRegisterGroup(IRefCount **pUnitsBuffer, int nLen) = 0;
  // unregistered group
  virtual void CommandUnregisterGroup(WORD wGroup) = 0;
  // send command to group of units
  virtual void CommandGroupCommand(const struct SAIUnitCmd *pCommand, WORD wGroup, bool bPlaceInQueue) = 0;
  // set single command to call planes, reinforcements, etc. 
  virtual int CommandUnitCommand(const struct SAIUnitCmd *pCommand) = 0;
  // show different areas... this command actually has been doing only on client, but it needs group to be registered
  virtual void CommandShowAreas(int nGroupID, int nAreaType, bool bShow) = 0;
  // process pCommand received in the current segment
  virtual void AddCommandToSend(interface IAILogicCommand *pCommand) = 0;

  // number of players currently playing
  virtual int GetNumberOfPlayers() const = 0;

  virtual bool JoinToServer(const char *pszIPAddress, int nPort, bool bPasswordRequired, const char *pszPassword) = 0;
  virtual void CreateServer() = 0;
  virtual void InitByCreateServersList() = 0;

  // client commands
  virtual void CommandClientTogglePause() = 0;
  virtual void CommandClientSpeed(int nChange) = 0;
  virtual void CommandClientDropPlayer(const wchar_t *pszPlayerNick) = 0;

  virtual void CommandTimeOut(bool bSet) = 0;

  virtual NTimer::STime GetMultiplayerTime() = 0;

  virtual void LoadAllGameParameters() = 0;

  virtual void SetTotalOutOfSync() = 0;

  virtual void GameFinished() = 0;

  // for debug
  virtual interface INetDriver * GetInGameNetDriver() const { return nullptr; }
};

#endif // __TRANSCEIVER_H__