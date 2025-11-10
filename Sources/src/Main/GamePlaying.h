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

  std::hash_map<int, int> clientID2LogicID;

  std::vector<BYTE> diplomacies;
  bool bStartGameReceived;

  //
  void RemoveClient(int nClientID);
  void ProcessPacket(int nClientID, IDataStream *pPkt);
  void UpdatePlayersInfo();
  void ProcessNewClient(int nClientID);

public:
  CGamePlaying() {}
  void STDCALL Init(INetDriver *pInGameNetDriver, INetDriver *pOutGameNetDriver, const CPlayers &players, bool bServer, int nOurID, const std::vector<BYTE> &diplomacies) override;

  IMultiplayer::CCommand * STDCALL GetCommand() override;
  void STDCALL SendClientCommands(IDataStream *pPacket) override;

  void STDCALL LeftGame() override;

  void STDCALL Segment() override;

  const bool STDCALL GetPlayerInfo(const WORD *pszPlayerName, SPlayerInfo *pInfo) const override;
  const bool STDCALL GetOurPlayerInfo(SPlayerInfo *pInfo) const override;

  const int STDCALL GetNAllies() const override;
  const SPlayerInfo & STDCALL GetAlly(int n) const override;

  int STDCALL GetNumberOfPlayers() const override;

  // client commands
  void STDCALL TogglePause() override;
  void STDCALL GameSpeed(int nChange) override;
  void STDCALL DropPlayer(int nLogicID) override;

  void STDCALL CommandTimeOut(bool bSet) override;

  void STDCALL SendAliveMessage() override;
  void STDCALL FinishGame() override;

  interface INetDriver * STDCALL GetInGameNetDriver() const override { return pInGameNetDriver; }
};

#endif // __GAME_PLAYING_H__