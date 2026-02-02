#ifndef __MULTIPLAYER_INTERNAL_H__
#define __MULTIPLAYER_INTERNAL_H__

#pragma once

#include "Multiplayer.h"

#include "../Net/NetDriver.h"

interface IServersList;
interface IGameCreation;
interface IChat;
interface IGamePlaying;

class CMultiplayer : public IMultiplayer
{
  EMultiplayerStates eState;
  int nParam;

  CPtr<IServersList> pServersList;
  CPtr<IGameCreation> pGameCreation;
  CPtr<IChat> pChat;
  CPtr<IGamePlaying> pGamePlaying;
  bool bInGSChat;

  NTimer::STime finishGameTime;

  //
  void ServersListSegment();
  void GameCreationSegment();
  void PlayingSegment();
  void CreateServer(const struct SGameInfo &gameInfo, const struct SQuickLoadMapInfo &mapInfo);
  void JoinToServer(WORD wServerID, bool bPasswordRequired, const std::string &szPassword);
  void StartGame();
  void ProcessChat();
  void SendChatMessageToConsole(interface IMultiplayerMessage *pMessage);

protected:
  virtual IServersList *CreateServersList() = 0;
  void SetGameCreation(IGameCreation *pGameCreation);
  void SetServersList(IServersList *pServersList);
  void SetState(EMultiplayerStates eState);
  void SetChat(IChat *pChat);
  IChat *GetChat();

public:
  CMultiplayer() : eState(EMS_NONE), bInGSChat(false) {}

  void InitServersList() override;
  void Segment() override;

  const EMultiplayerStates GetState() override { return eState; }

  CCommand * GetCommand() override;
  void SendClientCommands(IDataStream *pPacket) override;
  void SendInGameChatMessage(const wchar_t *pszType, const wchar_t *pszMessage) override;

  // valid onlye during of the game
  int GetNumberOfPlayers() const override;

  // client commands
  void TogglePause() override;
  void GameSpeed(int nChange) override;
  void DropPlayer(int nLogicID) override;

  void CommandTimeOut(bool bSet) override;

  void SendAliveMessage() override;
  void FinishGame() override;

  interface INetDriver * GetInGameNetDriver() const override;
};

class CLanMultiplayer : public CMultiplayer
{
  OBJECT_COMPLETE_METHODS(CLanMultiplayer);

protected:
  IServersList *CreateServersList() override;

public:
  void Init() override {}
  bool InitJoinToServer(const char *pszIPAddress, const int nPort, bool bPasswordRequired, const char *pszPassword) override { return true; }
};

class CInternetMultiplayer : public CMultiplayer
{
  OBJECT_COMPLETE_METHODS(CInternetMultiplayer);

protected:
  IServersList *CreateServersList() override;

public:
  CInternetMultiplayer() {}
  void Init() override {}
  bool InitJoinToServer(const char *pszIPAddress, const int nPort, bool bPasswordRequired, const char *pszPassword) override { return true; }
};


#endif // __MULTIPLAYER_INTERNAL_H__