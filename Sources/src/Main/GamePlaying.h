#ifndef __GAME_PLAYING_H__
#define __GAME_PLAYING_H__

#pragma once

#include "GameCreationInterfaces.h"

interface INetDriver;
class IMultiplayer::CCommand;

class CGamePlaying : public IGamePlaying
{
  OBJECT_COMPLETE_METHODS(CGamePlaying);

  CPtr<INetDriver> pInGameNetDriver;
  CPtr<INetDriver> pOutGameNetDriver;

  std::list<CPtr<IMultiplayer::CCommand>> commands;
  CPtr<IMultiplayer::CCommand> pTakenCommand;

  CPlayers players;
  int nOurID;
  std::vector<bool> lags;

  std::unordered_map<int, int> clientID2LogicID;

  std::vector<BYTE> diplomacies;
  bool bStartGameReceived;

  //
  void RemoveClient(int nClientID);
  void ProcessPacket(int nClientID, IDataStream *pPkt);
  void UpdatePlayersInfo();
  void ProcessNewClient(int nClientID);

public:
  CGamePlaying() {}
  void Init(INetDriver *pInGameNetDriver, INetDriver *pOutGameNetDriver, const CPlayers &players, bool bServer, int nOurID, const std::vector<BYTE> &diplomacies) override;

  IMultiplayer::CCommand * GetCommand() override;
  void SendClientCommands(IDataStream *pPacket) override;

  void LeftGame() override;

  void Segment() override;

  const bool GetPlayerInfo(const wchar_t *pszPlayerName, SPlayerInfo *pInfo) const override;
  const bool GetOurPlayerInfo(SPlayerInfo *pInfo) const override;

  const int GetNAllies() const override;
  const SPlayerInfo & GetAlly(int n) const override;

  int GetNumberOfPlayers() const override;

  // client commands
  void TogglePause() override;
  void GameSpeed(int nChange) override;
  void DropPlayer(int nLogicID) override;

  void CommandTimeOut(bool bSet) override;

  void SendAliveMessage() override;
  void FinishGame() override;

  interface INetDriver * GetInGameNetDriver() const override { return pInGameNetDriver; }
};

#endif // __GAME_PLAYING_H__