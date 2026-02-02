#ifndef __MULTIPLAYER_H__
#define __MULTIPLAYER_H__

#pragma once

#include "iMainClassIDs.h"

interface IAILogicCommand;

interface IMultiplayer : IRefCount
{
  enum { tidTypeID = LAN_MULTIPLAYER };

  enum EGamePlayingCommands
  {
    GPC_SEGMENT_FINISHED,
    GPC_PLAYER_REMOVED,
    GPC_AI_COMMAND,
    GPC_START_GAME,
    GPC_PLAYER_LAG,
    GPC_PAUSE,
    GPC_GAME_SPEED,
    GPC_TIMEOUT,
    GPC_PLAYER_ALIVE,
  };

  enum EMultiplayerStates { EMS_SERVERS_LIST, EMS_GAME_CREATION, EMS_PLAYING, EMS_NONE };

  class CCommand : public IRefCount
  {
    OBJECT_COMPLETE_METHODS(CCommand);

  public:
    EGamePlayingCommands eCommandType;
    CPtr<IAILogicCommand> pAILogicCommand;
    int nPlayer;
    int nParam;

    CCommand() : nPlayer(-1) {}

    CCommand(EGamePlayingCommands _eCommandType, const int _nPlayer, const int _nParam, IAILogicCommand *_pAILogicCommand)
      : eCommandType(_eCommandType), pAILogicCommand(_pAILogicCommand), nPlayer(_nPlayer), nParam(_nParam) {}
  };

  virtual void Init() = 0;
  virtual void Segment() = 0;
  virtual void InitServersList() = 0;
  virtual bool InitJoinToServer(const char *pszIPAddress, int nPort, bool bPasswordRequired, const char *pszPassword) = 0;

  virtual const EMultiplayerStates GetState() = 0;

  virtual CCommand * GetCommand() = 0;
  virtual void SendClientCommands(IDataStream *pPacket) = 0;
  virtual void SendInGameChatMessage(const wchar_t *pszType, const wchar_t *pszMessage) = 0;

  // valid onlye during of the game
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

#endif // __MULTIPLAYER_H__