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

  void STDCALL InitServersList() override;
  void STDCALL Segment() override;

  const EMultiplayerStates GetState() override { return eState; }

  CCommand * STDCALL GetCommand() override;
  void STDCALL SendClientCommands(IDataStream *pPacket) override;
  void STDCALL SendInGameChatMessage(const WORD *pszType, const WORD *pszMessage) override;

  // valid onlye during of the game
  int STDCALL GetNumberOfPlayers() const override;

  // client commands
  void STDCALL TogglePause() override;
  void STDCALL GameSpeed(int nChange) override;
  void STDCALL DropPlayer(int nLogicID) override;

  void STDCALL CommandTimeOut(bool bSet) override;

  void STDCALL SendAliveMessage() override;
  void STDCALL FinishGame() override;

  interface INetDriver * STDCALL GetInGameNetDriver() const override;
};

class CLanMultiplayer : public CMultiplayer
{
  OBJECT_COMPLETE_METHODS(CLanMultiplayer);

protected:
  IServersList *CreateServersList() override;

public:
  void STDCALL Init() override {}
  bool STDCALL InitJoinToServer(const char *pszIPAddress, const int nPort, bool bPasswordRequired, const char *pszPassword) override { return true; }
};

class CGameSpyMultiplayer : public CMultiplayer
{
  OBJECT_COMPLETE_METHODS(CGameSpyMultiplayer);

protected:
  IServersList *CreateServersList() override;

public:
  CGameSpyMultiplayer();
  void STDCALL Init() override;

  void STDCALL InitServersList() override;
  bool STDCALL InitJoinToServer(const char *pszIPAddress, int nPort, bool bPasswordRequired, const char *pszPassword) override;
};

class CInternetMultiplayer : public CMultiplayer
{
  OBJECT_COMPLETE_METHODS(CInternetMultiplayer);

protected:
  IServersList *CreateServersList() override;

public:
  CInternetMultiplayer() {}
  void STDCALL Init() override {}
  bool STDCALL InitJoinToServer(const char *pszIPAddress, const int nPort, bool bPasswordRequired, const char *pszPassword) override { return true; }
};

#endif // __MULTIPLAYER_INTERNAL_H__