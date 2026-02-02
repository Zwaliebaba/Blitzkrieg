#ifndef __GAME_CREATION_INTERFACES_H__
#define __GAME_CREATION_INTERFACES_H__

#pragma once

#include "ServerInfo.h"
#include "Multiplayer.h"

interface IChat;

interface IServersList : IRefCount
{
  virtual interface IGameCreation * CreateServer(const struct SGameInfo &gameInfo, const struct SQuickLoadMapInfo &mapInfo, CPtr<IChat> *pChat) = 0;
  virtual bool CanJoinToServerByID(WORD wServerID) = 0;
  virtual bool IsNeedPassword(WORD wServerID) const = 0;
  virtual interface IGameCreation * JoinToServerByID(WORD wServerID, CPtr<IChat> *pChat, bool bPasswordRequired, const std::string &szPassword) = 0;
  virtual interface IGameCreation * JoinToServerByAddress(interface INetNodeAddress *pAddress, CPtr<IChat> *pChat, int nPort, bool bPasswordRequired, const std::string &szPassword) = 0;

  virtual interface IMultiplayerMessage * GetMessage() = 0;
  virtual void Segment() = 0;
  virtual void Refresh() = 0;

  virtual interface INetDriver * GetInGameNetDriver() const = 0;
};

interface IGameCreation : IRefCount
{
  enum EPlayerSettings { EPS_READY, EPS_SIDE, EPS_NAME, EPS_MAP_LOAD_PROGRESS };

  virtual void LeftGame() = 0;
  virtual void KickPlayer(int nLogicID) = 0;
  virtual void ChangeGameSettings() = 0;
  virtual void ChangePlayerSettings(const struct SPlayerInfo &info, const EPlayerSettings &eSettingsType) = 0;

  virtual void Launch() = 0;

  virtual interface IMultiplayerMessage * GetMessage() = 0;
  virtual void Segment() = 0;

  virtual bool CanStartGame() const = 0;
  virtual bool IsAllPlayersInOneParty() const = 0;
  virtual interface IGamePlaying * CreateGamePlaying() = 0;

  virtual const bool GetPlayerInfo(const wchar_t *pszPlayerName, SPlayerInfo *pInfo) const = 0;
  virtual const bool GetOurPlayerInfo(SPlayerInfo *pInfo) const = 0;
  virtual void SetNewGameSettings(const SMultiplayerGameSettings &settings) = 0;
  virtual void ModChanged() {}

  virtual interface INetDriver * GetInGameNetDriver() const = 0;
};

interface IAILogicCommand;
interface IGamePlaying : IRefCount
{
  virtual void Init(interface INetDriver *pInGameNetDriver, interface INetDriver *pOutGameNetDriver,
                            const CPlayers &players, bool bServer, int nOurID,
                            const std::vector<BYTE> &diplomacies) = 0;

  virtual IMultiplayer::CCommand * GetCommand() = 0;
  virtual void SendClientCommands(IDataStream *pPacket) = 0;
  virtual void LeftGame() = 0;

  virtual void Segment() = 0;

  virtual const bool GetPlayerInfo(const wchar_t *pszPlayerName, SPlayerInfo *pInfo) const = 0;
  virtual const bool GetOurPlayerInfo(SPlayerInfo *pInfo) const = 0;

  virtual const int GetNAllies() const = 0;
  virtual const SPlayerInfo & GetAlly(int n) const = 0;

  virtual int GetNumberOfPlayers() const = 0;

  // client commands
  virtual void TogglePause() = 0;
  virtual void GameSpeed(int nChange) = 0;
  virtual void DropPlayer(int nLogicID) = 0;

  virtual void CommandTimeOut(bool bSet) = 0;

  virtual void SendAliveMessage() = 0;
  virtual void FinishGame() = 0;

  virtual interface INetDriver * GetInGameNetDriver() const = 0;
};

interface IChat : IRefCount
{
  enum EUserMode
  {
    EUM_NONE,
    EUM_AWAY,
    EUM_NOT_AWAY,
    EUM_IN_CHAT,
    EUM_IN_SERVERS_LIST,
    EUM_IN_STAGING_ROOM,
    EUM_IN_GAME_PLAYING,
  };

  virtual void InitGSChat(const wchar_t *pszUserName) = 0;
  virtual void InitInGameChat(INetDriver *pNetDriver) = 0;
  virtual void DestroyInGameChat() = 0;

  virtual void SendMessage(const wchar_t *pszMessage, const SPlayerInfo &ourPlayer) = 0;
  virtual void SendWhisperMessage(const wchar_t *pszMessage, const SPlayerInfo &toPlayer, const SPlayerInfo &ourPlayer) = 0;
  // for gamespy messages
  virtual void SendMessage(const wchar_t *pszMessage, const wchar_t *wszToPlayer, bool bWhisper) = 0;

  virtual void Segment() = 0;

  virtual interface IMultiplayerMessage * GetMessage() = 0;

  virtual void UserModeChanged(EUserMode eMode) = 0;
};

#endif // __GAME_CREATION_INTERFACES_H__